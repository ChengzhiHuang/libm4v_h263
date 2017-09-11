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

// Constants.
enum{
    kMaxWidth         = 1920,
    kMaxHeight        = 1080,
};


int main(int argc, char *argv[]) {

    if (argc < 6) {
        fprintf(stderr, "Usage %s <input file> <output yuv420> <mode> <width> "
                "<height> <(optional)enable debug message>\n", argv[0]);
        fprintf(stderr, "mode : h263 or mpeg4\n");
        fprintf(stderr, "For now , we only support mpeg4 for SP:No B-frame ,one layer\n");
        fprintf(stderr, "BTW , Your mpeg4 file must include VO&VOL header\n");
        fprintf(stderr, "Max width %d\n", kMaxWidth);
        fprintf(stderr, "Max height %d\n", kMaxHeight);
        fprintf(stderr, "Default has no debug message\n");
        return EXIT_FAILURE;
    }

    // Initialize the encoder parameters.
    // Read mode.
    MP4DecodingMode decMode;
    if (strcmp(argv[3], "mpeg4") == 0) {
        decMode = MPEG4_MODE;
    } else if (strcmp(argv[3], "h263") == 0) {
        decMode = H263_MODE;
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

    // Read if enable debug output.
    bool enableDebug = false;
    if(argc == 7 && strcmp(argv[6], "true") == 0){
        enableDebug = true;
    }

    // Initialize the handle.
    VideoDecControls decCtrl;
    memset(&decCtrl,0,sizeof(VideoDecControls));

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
        fclose(fpInput);
        return EXIT_FAILURE;
    }

    // Set the layer.
    int nLayers = 1;

    // Set the VOL buffer & size.
    int32_t bytes = fread(inputBuf, 1, 100, fpInput);
    int32 *volbuf_size = new int32[nLayers]{bytes};
    uint8 **volbuf = new uint8*[nLayers]{ inputBuf };

    // Initialize the encoder & deal with the VOL header.
    if(!PVInitVideoDecoder(&decCtrl,volbuf,volbuf_size,nLayers,width,height,decMode)){
        fprintf(stderr, "PVInitVideoDecoder failed. Unsupported content?\n");
        free(inputBuf);
        free(outputBuf);
        free(lastFrameBuf);
        fclose(fpInput);
        fclose(fpOutput);
        delete []volbuf_size;
        delete []volbuf;
        return EXIT_FAILURE;
    }

    // Variables for decode helps.
    int32_t numFramesDecoded = 0;
    long nextFrameBeginPoint = 0;
    uint8_t* tempBuffer = 0;

    // Set timing variables.
    clock_t start, finish;
    if(enableDebug){
        start = clock();
    }

    while (1) {
        // Read the input header & frame.

        // Do the parser's work :Find the VOP header.
        // Then put it from the inputBuf to inputFrame.
        int32_t bytesRead;
        int32_t frameSize = 250 * 1024;
        fseek(fpInput,nextFrameBeginPoint,0);
        bytesRead = fread(inputBuf, 1, frameSize, fpInput);
        if (bytesRead == 0) {
            break; // End of file.
        }

        // Set the Reference frame only Once.
        if(numFramesDecoded == 0){
            PVSetReferenceYUV(&decCtrl, lastFrameBuf);
        }

        // Decode the input frame.We don't care about the audio.
        uint32 timestamp = 0xFFFFFFFF;
        uint use_ext_timestamp = 0;
        if(PVDecodeVideoFrame(&decCtrl,&inputBuf,&timestamp,&bytesRead,&use_ext_timestamp,outputBuf) != PV_TRUE){
            fprintf(stderr, "Failed to decode frame %d \n",numFramesDecoded);
            free(inputBuf);
            free(outputBuf);
            free(lastFrameBuf);
            fclose(fpInput);
            fclose(fpOutput);
            delete []volbuf_size;
            delete []volbuf;
            return EXIT_FAILURE;
        }

        // Check the next frames's VOP header begin point.
        VideoDecData *video = (VideoDecData *) decCtrl.videoDecoderData;
        nextFrameBeginPoint += ( video->bitstream->bitcnt / 8 );

        // Write the output.
        fwrite(outputBuf, 1, dataLength, fpOutput);

        // Exchange the buffer pointer.
        tempBuffer = lastFrameBuf;
        lastFrameBuf = outputBuf;
        outputBuf = tempBuffer;

        if(enableDebug)
            printf("The %d frame has been decoded! \n",numFramesDecoded++);
    }
    if(enableDebug){
        finish = clock();
        printf("numFramesDecoded : %d \n",numFramesDecoded);
        double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf("Whole time is %f , average fps is %f \n", duration ,1 / (duration/numFramesDecoded));
    }

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
    PVCleanUpVideoDecoder(&decCtrl);
}
