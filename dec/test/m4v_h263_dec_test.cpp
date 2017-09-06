//
// Created by chengzhi on 8/27/17.
//

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <bitstream.h>
#include <zconf.h>
#include "post_proc.h"

int main(int argc, char *argv[]) {
//    bool isH263mode;
    // Initialize the encoder parameters.
//    VideoDecControls* decCtrl = (VideoDecControls*)malloc(sizeof(VideoDecControls));
//    oscl_memset(decCtrl, 0, sizeof(VideoDecControls));

    VideoDecControls* decCtrl = new VideoDecControls;
//    oscl_memset(decCtrl, 0, sizeof(VideoDecControls));
//    printf("decCtrl->readBitstreamData: %x \n",decCtrl->readBitstreamData);

    int width = 368;
    int height = 208;

    int nLayers = 1;
    // Allocate input buffer.
    uint8_t *inputBuf = (uint8_t *)malloc(250 * 1024);
    memset(inputBuf,0,250 * 1024);
    assert(inputBuf != NULL);

    // Allocate output buffer.
    uint8_t *outputBuf = (uint8_t *)malloc((width * height * 3) / 2);
    memset(outputBuf,0,(width * height * 3) / 2);
    assert(outputBuf != NULL);

    // Allocate last frame buffer.
    uint8_t *lastFrameBuf = (uint8_t *)malloc((width * height * 3) / 2);
    memset(lastFrameBuf,0,(width * height * 3) / 2);
    assert(lastFrameBuf != NULL);

    // Open the input file.
    FILE *fpInput = fopen(argv[1], "rb");
    if (fpInput == NULL) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        free(inputBuf);
        free(outputBuf);
        return EXIT_FAILURE;
    }

    // Open the output file.
    FILE *fpOutput = fopen(argv[2], "wb");
    if (fpOutput == NULL) {
        fprintf(stderr, "Could not open %s\n", argv[2]);
        free(inputBuf);
        free(outputBuf);
        fclose(fpInput);
        return EXIT_FAILURE;
    }
    int32_t bytes = fread(inputBuf, 1, 500, fpInput);
    int32* volbuf_size = new int32[1]{bytes};
    uint8 **volbuf = new uint8*[nLayers]{ inputBuf };

//    //Initialize the encoder & deal with the VOL header
    if(!PVInitVideoDecoder(decCtrl,volbuf,volbuf_size,nLayers,width,height,MPEG4_MODE)){
        fprintf(stderr, "PVInitVideoDecoder failed. Unsupported content?\n");
        return EXIT_FAILURE;
    }
    PVSetPostProcType((VideoDecControls *) decCtrl, 0);

    int32_t numFramesDecoded = 0;
    long nextFrameBeginPoint = 0;
    uint8_t* tempbuffer = 0;
    while (1) {

        printf("Begin decode the  %d frame ! \n",numFramesDecoded);


        // Read the input header & frame.

        // Do the parser's work :Find the VOP header.
        // Then put it from the inputBuf to inputFrame
        int32_t bytesRead;
        int32_t frameSize = 250 * 512;
        fseek(fpInput,nextFrameBeginPoint,0);
        bytesRead = fread(inputBuf, 1, frameSize, fpInput);
        if (bytesRead != frameSize) {
            break; // End of file.
        }

        // Set the Reference frame
        if(numFramesDecoded == 0){
            PVSetReferenceYUV(decCtrl, lastFrameBuf);
        }

        // Decode the input frame.
        uint32 timestamp = 0xFFFFFFFF;
        int32_t dataLength = width * height * 3 /2;
        uint use_ext_timestamp = 0;

        printf("Before Decode:\n prevVop: %ld \n currVop: %ld \n\n\n",((VideoDecData *)decCtrl->videoDecoderData)->prevVop->yChan ,
               ((VideoDecData *)decCtrl->videoDecoderData)->currVop->yChan);

        if(PVDecodeVideoFrame(decCtrl,&inputBuf,&timestamp,&bytesRead,&use_ext_timestamp,outputBuf) != PV_TRUE){
            fprintf(stderr, "Failed to decode frame %d \n",numFramesDecoded);
            return EXIT_FAILURE;
        }
        printf("After Decode:\n prevVop: %ld \n currVop: %ld \n\n\n",((VideoDecData *)decCtrl->videoDecoderData)->prevVop->yChan ,
               ((VideoDecData *)decCtrl->videoDecoderData)->currVop->yChan);


        VideoDecData *video = (VideoDecData *) decCtrl->videoDecoderData;
//        nextFrameBeginPoint += video->bitstream->read_point - 6 ;
        nextFrameBeginPoint += ( video->bitstream->bitcnt / 8 );
//        memcpy(lastFrameBuf,outputBuf,dataLength);//单位是byte,不用转换
        // Write the output.
        fwrite(outputBuf, 1, dataLength, fpOutput);

        tempbuffer = lastFrameBuf;
        lastFrameBuf = outputBuf;
        outputBuf = tempbuffer;

//        memset(lastFrameBuf,0,dataLength);
//        int fd = ::fileno(fpOutput); //获取文件描述符
//        ::fsync(fd); //强制写硬盘
        printf("The %d frame has been decoded! \n ////////////////////////////////////////////////// \n\n",numFramesDecoded++);
//        printf("The frame %d nextFrameBeginPoint is %ld (byte)! \n\n\n\n",numFramesDecoded,nextFrameBeginPoint);
    }
    printf("numFramesDecoded : %d",numFramesDecoded);

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
