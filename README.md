# libm4v_h263
------

## Intruction
<br/>Forked from [AOSP's git](https://android.googlesource.com/platform/external/libavc).
<br/>The encode part can run on Ubantu x86 platforms using cmake.
<br/>The decode part need a test file and I'll add it in the near future.

------


### Building Instructions
<br/>You should download other 5 repositories : `av` , `core` , `libhardware` , `libhidl` , `native`.
<br/>Add the soft link to them under `enc/frameworks` , `dec/frameworks` .
<br/>The directory frameworks should be created by yourself.
<br/>The directory is like this :
```Bash
chengzhi@chengzhi-Kabylake-Client-platform:~/code/libm4v_h263/dec/frameworks$ ls -l
总用量 0
lrwxrwxrwx 1 chengzhi video 22 Aug 21 22:40 av -> /home/chengzhi/code/av
lrwxrwxrwx 1 chengzhi video 24 Aug 22 14:31 core -> /home/chengzhi/code/core
lrwxrwxrwx 1 chengzhi video 32 Aug 22 18:41 libhardware -> /home/chengzhi/code/libhardware/
lrwxrwxrwx 1 chengzhi video 27 Aug 22 18:58 libhidl -> /home/chengzhi/code/libhidl
lrwxrwxrwx 1 chengzhi video 27 Aug 22 13:16 native -> /home/chengzhi/code/native/
```
<br/>Then went into the working directory.
```Bash
cmake .
make
```
<br/>Then the execable file will be generated into  ./bin.
<br/>All the libs needed by building ,such as : `libutils.so` , `libmedia.so`  ,are generated under the x86_Android ,you can use it directly.

------
### Something About MPEG4 's Header
<br/>当前库是必须得有VO header -> VOL header -> VOP header这个过程才能解析每一帧 <VOP header , VOP data>.关于MPEG4结构的`教程1`看[这里](http://blog.sina.com.cn/s/blog_628bebe40101blly.html)里面的`MPEG-4码流层次化结构图`。然而现在使用ffmpeg从mp4文件中获得mpeg4裸流的话，是没有VO header 和VOL header的，得自己加上去。`教程2`看[这里](http://processors.wiki.ti.com/index.php/Extracting_MPEG-4_Elementary_Stream_from_MP4_Container).
<br/>然后顺便对比一下跟[mp4def.h](github.com/ChengzhiHuang/libm4v_h263/dec/src/mp4def.h)的一些定义的对比。

#### 初步概念
<br/>首先是解释一些基本概念，8 bit(b) = 1 byte(B),`0x`开头的数字就是表示是16进制(hex)的,所以一个hex的数，就是需要0.5 byte = 4 bit的长度，所以我们从16进制的文件阅读器`GHex`看到的段是这样的
>> 00 00 01 01 00 00 01 20 00 84 40 FA 28 A0 21 E0

<br/>用一个空格分割的两个数字就是16进制表示的一个byte。


#### VO + VOL header生成
<br/>下面是`教程2`的一些代码,我看得时候遇到很多障碍,所以现在我解释一下.
```c++
/* video_object_start_code */
BITS_putULong(bits, 0x00000101, 32);
 
/* video_object_layer_start_code */
BITS_putULong(bits, 0x00000120, 32);
```
<br/>第一个就是写VO header的，接着再写VOL header。
<br/>以第一行为例，BITS_putULong的第一个参数是写入的单位;第二个参数是写入的字符，0x是表示写入的是16进制的数字,`00 00 01 01`实际上写入了`0000 0000 0000 0000 0000 0001 0000 0001`的二进制流;第三个参数就是写入的长度，如果写入的长度大于实际要写入的第二个参数，就在左边补0。下面会有例子说明。
<br/>最后再把4个二进制拼成1个16进制,再把两个16进制放在一起成为1个byte看起来方便，因此这两行写入的从16进制来看就是： `00 00 01 01 00 00 01 20`

```c
/* random_accessible_vol */
BITS_put(bits, 0x0, 1);
 
/* video_object_type_indication */
BITS_put(bits, 0x01, 8);
 
/* is_object_layer_identifier */
BITS_put(bits, 0x0, 1);
 
/* aspect_ratio_info */
BITS_put(bits, 0x1, 4);
 
/* vol_control_parameters */
BITS_put(bits, 0x0, 1);
 
/* video_object_layer_shape */
BITS_put(bits, 0x0, 2);
 
/* marker_bit */
BITS_put(bits, 0x1, 1);
```

<br/>`BITS_put()`, `BITS_putUShort()` and `BITS_putULong()`没有多大差别,就是能写入的最大位数(也就是第三个参数)有限制,分别是8,16,32个bit.所以不用去在乎这个的变化.
<br/>第一个就写16进制数字0,到1个bit的空位里去,所以写的就是 : `0`.同时由于这里的写入的只有一个bit,不是4的倍数,因此就不能直接像上面那样直接写输出了,得用2进制拼起来.
<br/>第二个就写16进制数字1,到8个bit的空位里去,左边多余的位数补0,所以写的就是 : `00000001`
<br/>以此类推,接下来的是 : `000010001`
<br/>以此这部分产生的2进制数就是: `0000 0000 1000 0100 01--`,转到16进制来看就是:`00 84 --`,最后一位由于还不完整,所以得不出结论.

```c
/* vop_time_increment_resolution */
BITS_putUShort(bits, vopTimeIncrementResolution, 16);
 
/* marker_bit */
BITS_put(bits, 0x1, 1);
 
/* fixed_vop_rate */
BITS_put(bits, 0x0, 1);
```

<br/>在这个例子里面vopTimeIncrementResolution=1000,所以第一行转成2进制数就是`11 1110 1000` .要填补16位的话,就需要填补成:`0000 0011 1110 1000`.
<br/>第二行第三行的二进制就是:`10`.
<br/>结合上面一部分空出来的01,最后就是`0100 0000 1111 1010 0010` ,转成16进制就是`40 FA 2-`.

<br/>大致就是能够自己生成Simple Profile的Header了.

#### 与这个`libm4v_h263`库的一些对比
```c
/* session layer and vop layer start codes */
#define VISUAL_OBJECT_SEQUENCE_START_CODE   0x01B0
#define VISUAL_OBJECT_SEQUENCE_END_CODE     0x01B1

#define VISUAL_OBJECT_START_CODE   0x01B5
#define VO_START_CODE           0x8
#define VO_HEADER_LENGTH        32      /* lengtho of VO header: VO_START_CODE +  VO_ID */

#define SOL_START_CODE          0x01BE
#define SOL_START_CODE_LENGTH   32

#define VOL_START_CODE 0x12
#define VOL_START_CODE_LENGTH 28

#define VOP_START_CODE 0x1B6
#define VOP_START_CODE_LENGTH   32

```
<br/>这里所有的length的默认单位都是指bit.我用[`mp4UI`](https://sourceforge.net/projects/mp4ui/?source=typ_redirect)从mp4流中提取出来的VOS+VO+VOP的流.
>> 00 00 01 B0 03 00 00 01 B5 09 00 00 01 00 00 00

>> 01 20 00 86 C4 00 67 0C 2E 10 68 51 8F 00 00 01

>> B6 10 60 91 82 41 B7 F1 B6 DF C6 DB 7F 1B 6D FC

<br/>首先是`VOS header`,`00 00 01 B0`,接着是`-profile_and_level_indication`的值，simple profile一般值为`0x3`.
<br/>接着是`VO header`的start code,`00 01 B5`
<br/>最后就是`VOL header`的start code,`00 00 01 20`这里他的VOP_START_CODE_LENGTH 和`教程2`里面的不太一样,留待后续研究吧.



### Other tricks
<br/>在设置 `-Werror`的情况下,如何在print指针地址?
<br/>经过两次转换,如下
```c
#include <cstdio>
uint BitstreamRead1Bits(BitstreamDecVideo *stream)
{
    printf("%x \n",(unsigned int)(unsigned int*) (stream));
}

```
<br/>一般来说,buffer的管理是应该封装在API中的,但是由于这个库只支持SP,也就意味着没有B-frame,而P-frame只会将前一帧作为参考帧,所以其实只需要1个input Buffer,两个output Buffer轮流转换即可.这个是需要自己在mian函数中进行管理的.
