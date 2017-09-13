//
// Created by chengzhi on 8/27/17.
//

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "mp4dec_api.h"
#include <mp4lib_int.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

// Constants.
enum{
    kMaxWidth         = 1920,
    kMaxHeight        = 1080,
};

class YuvRender{

public:
    YuvRender(int x , int y ,int width , int height){
        // Initialize SDL
        SDL_Init(SDL_INIT_VIDEO);
        // Clean up on exit
        atexit(SDL_Quit);

        win = SDL_CreateWindow("m4v_h263_dec_test", x, y, width, height, SDL_WINDOW_SHOWN);
        if (win == nullptr){
            printf("SDL_CreateWindow Error: %s \n",SDL_GetError()) ;
            SDL_Quit();
            exit(1);
        }

        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (ren == nullptr){
            SDL_DestroyWindow(win);
            printf("SDL_CreateRenderer Error: %s \n",SDL_GetError());
            SDL_Quit();
            exit(1);
        }
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        texture =SDL_CreateTexture(ren, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
        rect = new SDL_Rect{x,y,width,height};
    }

    bool Render(unsigned char* buffer){
        if(SDL_PollEvent(&event)){
            printf("SDL_Event: %d \n",event.type);
        }

        SDL_UpdateTexture(texture, NULL, buffer, rect->w);

//        void * pixel = NULL;
//        int pitch = 0;
//        if(0 == SDL_LockTexture(texture, NULL, &pixel, &pitch))
//        {
//            // 如果不考虑数据对齐，直接拷贝YUV数据是没有问题的
//            if (pitch == rect->w)
//            {
//                memcpy(pixel, buffer, (rect->w * rect->h * 3) / 2);
//            }
//            else // 可能发生pitch > width的情况
//            {
//                // 如果有数据对齐的情况，单独拷贝每一行数据
//                // for Y
//                int h = rect->h;
//                int w = rect->w;
//                unsigned char * dst = reinterpret_cast<unsigned char *>(pixel);
//                unsigned char * src = buffer;
//                for (int i = 0; i < h; ++i)
//                {
//                    memcpy(dst, src, w);
//                    dst += pitch;
//                    src += w;
//                }
//
//                h >>= 1;
//                w >>= 1;
//                pitch >>= 1;
//                // for U
//                for (int i = 0; i < h; ++i)
//                {
//                    memcpy(dst, src, w);
//                    dst += pitch;
//                    src += w;
//                }
//
//                // for V
//                for (int i = 0; i < h; ++i)
//                {
//                    memcpy(dst, src, w);
//                    dst += pitch;
//                    src += w;
//                }
//            }
//            SDL_UnlockTexture(texture);
//        }
//        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, texture, NULL, NULL);
//        SDL_Delay(200);
        SDL_RenderPresent(ren);
        return true;
    }

    ~YuvRender(){
        delete rect;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }

private:
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    SDL_Texture *texture = NULL;
    SDL_Rect *rect = NULL;

    SDL_Event event;//事件
};

int main(int argc, char *argv[]) {

    if (argc < 5) {
        fprintf(stderr, "Usage %s <input file> <output yuv420> <mode> <width> "
                "<height>\n", argv[0]);
        fprintf(stderr, "mode : h263 or mpeg4\n");
        fprintf(stderr, "For now , we only support mpeg4 for SP:No B-frame ,one layer \n");
        fprintf(stderr, "BTW , Your mpeg4 file must include VO&VOL header\n");
        fprintf(stderr, "Max width %d\n", kMaxWidth);
        fprintf(stderr, "Max height %d\n", kMaxHeight);
        return EXIT_FAILURE;
    }
    // Initialize the encoder parameters.
    // Read mode.
    bool isH263mode;
    if (strcmp(argv[3], "mpeg4") == 0) {
        isH263mode = false;
    } else if (strcmp(argv[3], "h263") == 0) {
        isH263mode = true;
    } else {
        fprintf(stderr, "Unsupported mode %s\n", argv[3]);
        return EXIT_FAILURE;
    }

    // Read height and width.
    int32_t width ,height;
    width = atoi(argv[4]);
    height = atoi(argv[5]);
    if (width > kMaxWidth || height > kMaxHeight || width <= 0 || height <= 0) {
        fprintf(stderr, "Unsupported dimensions %dx%d\n", width, height);
        return EXIT_FAILURE;
    }

    // Initialize the handle.
    VideoDecControls* decCtrl = new VideoDecControls;
    memset(decCtrl,0,sizeof(VideoDecControls));

    // Allocate input buffer.
    uint8_t *inputBuf = (uint8_t *)malloc(250 * 1024);
    memset(inputBuf,0,250 * 1024);
    assert(inputBuf != NULL);

    // Allocate output buffer.
    int32_t dataLength = width * height * 3 /2;
    uint8_t *outputBuf = (uint8_t *)malloc(dataLength);
    memset(outputBuf,0,dataLength);
    assert(outputBuf != NULL);

    // Allocate last frame buffer.
    uint8_t *lastFrameBuf = (uint8_t *)malloc(dataLength);
    memset(lastFrameBuf,0,dataLength);
    assert(lastFrameBuf != NULL);

    // Open the input file.
    FILE *fpInput = fopen(argv[1], "rb");
    if (fpInput == NULL) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        free(inputBuf);
        free(outputBuf);
        free(lastFrameBuf);
        return EXIT_FAILURE;
    }

    // Open the output file.
    FILE *fpOutput = fopen(argv[2], "wb");
    if (fpOutput == NULL) {
        fprintf(stderr, "Could not open %s\n", argv[2]);
        free(inputBuf);
        free(outputBuf);
        free(lastFrameBuf);
        fclose(fpOutput);
        return EXIT_FAILURE;
    }

    YuvRender yuvRender(0,0,width,height);

    // Set the layer.
    int nLayers = 1;

    // Set the VOL buffer & size.
    int32_t bytes = fread(inputBuf, 1, 100, fpInput);
    int32* volbuf_size = new int32[nLayers]{bytes};
    uint8 **volbuf = new uint8*[nLayers]{ inputBuf };

    // Initialize the encoder & deal with the VOL header.
    if(!PVInitVideoDecoder(decCtrl,volbuf,volbuf_size,nLayers,width,height,MPEG4_MODE)){
        fprintf(stderr, "PVInitVideoDecoder failed. Unsupported content?\n");
        return EXIT_FAILURE;
    }

    // Variables for decode helps.
    int32_t numFramesDecoded = 0;
    long nextFrameBeginPoint = 0;
    uint8_t* tempBuffer = 0;

    // Set timing variables.
    clock_t start, finish;
    start = clock();
    while (1) {
        // Read the input header & frame.

        // Do the parser's work :Find the VOP header.
        // Then put it from the inputBuf to inputFrame
        int32_t bytesRead;
        int32_t frameSize = 250 * 1024;
        fseek(fpInput,nextFrameBeginPoint,0);
        bytesRead = fread(inputBuf, 1, frameSize, fpInput);
        if (bytesRead == 0) {
            break; // End of file.
        }

        // Set the Reference frame only Once.
        if(numFramesDecoded == 0){
            PVSetReferenceYUV(decCtrl, lastFrameBuf);
        }

        // Decode the input frame.We don't care about the audio.
        uint32 timestamp = 0xFFFFFFFF;
        uint use_ext_timestamp = 0;
        if(PVDecodeVideoFrame(decCtrl,&inputBuf,&timestamp,&bytesRead,&use_ext_timestamp,outputBuf) != PV_TRUE){
            fprintf(stderr, "Failed to decode frame %d \n",numFramesDecoded);
            return EXIT_FAILURE;
        }

        // Check the next frames's VOP header begin point.
        VideoDecData *video = (VideoDecData *) decCtrl->videoDecoderData;
        nextFrameBeginPoint += ( video->bitstream->bitcnt / 8 );

        yuvRender.Render(outputBuf);

        // Write the output.
        fwrite(outputBuf, 1, dataLength, fpOutput);

        // Exchange the buffer pointer.
        tempBuffer = lastFrameBuf;
        lastFrameBuf = outputBuf;
        outputBuf = tempBuffer;

        printf("The %d frame has been decoded! \n",numFramesDecoded++);
    }
    finish = clock();
    printf("numFramesDecoded : %d \n",numFramesDecoded);
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("Whole time is %f , average fps is %f \n", duration ,1 / (duration/numFramesDecoded));

    delete []volbuf_size;
    delete []volbuf;

    // Close input and output file.
    fclose(fpInput);
    fclose(fpOutput);

    // Free allocated memory.
    free(inputBuf);
    free(lastFrameBuf);
    free(outputBuf);

    // Close encoder instance.
    PVCleanUpVideoDecoder(decCtrl);
}
