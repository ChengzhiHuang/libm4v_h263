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
