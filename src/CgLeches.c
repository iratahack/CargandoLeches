#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

unsigned char in[0x10000], mem[0x20000];
int i, tlength;
uint16_t length, frequency = 44100;
char tzx = 0, channel_type = 1, velo = 3, offset = 0, command[0x100], *ext;

int main(int argc, char *argv[])
{
    FILE *inFile, *outFile, *tmpFile;
    char *path = dirname(argv[0]);

    if ((argc < 3) || (argc > 7))
        printf("\n"
               "CgLeches v1.00, an ultra load block generator by Antonio Villena, 17 Sep 2014\n\n"
               "  CgLeches <ifile> <ofile> [speed] [channel_type] [srate] [offset]\n\n"
               "  <ifile>        TAP input file, mandatory\n"
               "  <ofile>        Output file, between TZX or WAV file, mandatory\n"
               "  [speed]        A number between 0 and 7. Lower is faster. Default 3\n"
               "  [channel_type] Possible values are: mono (default), stereo or stereoinv\n"
               "  [srate]        Sample rate, 44100 (default) or 48000\n"
               "  [offset]       -2,-1,0 (default),1 or 2. Fine grain adjust for symbol offset\n\n"),
            exit(0);

    if ((inFile = fopen(argv[1], "rb")) == NULL)
        perror(argv[1]),
            exit(-1);

    fseek(inFile, 0, SEEK_END);
    tlength = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    if (!(ext = strrchr(argv[2], '.')))
        printf("%s: Output file name must have .wav or .tzx extension\n", argv[2]),
            exit(-1);

    if ((outFile = fopen(argv[2], "wb+")) == NULL)
        perror(argv[2]),
            exit(-1);

    if (argc > 3)
    {
        velo = atoi(argv[3]);
        if (velo > 7)
            printf("\nInvalid speed: %s\n", argv[3]),
                exit(-1);
    }
    if (argc > 4)
    {
        if (!strcasecmp(argv[4], "mono"))
            channel_type = 1;
        else if (!strcasecmp(argv[4], "stereo"))
            channel_type = 2;
        else if (!strcasecmp(argv[4], "stereoinv"))
            channel_type = 6;
        else
            printf("\nInvalid argument name: %s\n", argv[4]),
                exit(-1);
    }
    if (argc > 5)
    {
        frequency = atoi(argv[5]);
        if (frequency != 44100 && frequency != 48000)
            printf("\nInvalid sample rate: %s\n", argv[5]),
                exit(-1);
    }
    if (argc > 6)
    {
        offset = atoi(argv[6]);
        if (offset < -2 || offset > 2)
            printf("\nInvalid offset: %s\n", argv[6]),
                exit(-1);
    }

    if (!strcasecmp(ext, ".tzx"))
    {
        fprintf(outFile, "ZXTape!");
        *(uint32_t *)mem = 0xa011a;
        fwrite(mem, 1, 3, outFile);
        tzx = 1;
    }
    else if (!strcasecmp(ext, ".wav"))
    {
        memset(mem, 0, 44);
        *(uint32_t *)mem = 0x46464952;
        *(uint32_t *)(mem + 8) = 0x45564157;
        *(uint32_t *)(mem + 12) = 0x20746d66;
        *(uint8_t *)(mem + 16) = 0x10;
        *(uint8_t *)(mem + 20) = 0x01;
        *(uint8_t *)(mem + 22) = *(uint8_t *)(mem + 32) = channel_type & 3;
        *(uint16_t *)(mem + 24) = frequency;
        *(uint32_t *)(mem + 28) = frequency * (channel_type & 3);
        *(uint8_t *)(mem + 34) = 8;
        *(uint32_t *)(mem + 36) = 0x61746164;
        fwrite(mem, 1, 44, outFile);
    }
    else
    {
        printf("%s: Invalid extension for output file name\n", ext);
        exit(-1);
    }

    ext++;

    while (tlength)
    {
        if (fread(&length, sizeof(length), 1, inFile) != 1)
            perror("reading length"),
                exit(-1);
        if (fread(in, 1, length, inFile) != length)
            printf("Error: Reading %d bytes\n", length),
                exit(-1);
        tmpFile = fopen("_tmp.tap", "wb+");
        fwrite(in + 1, 1, length - 2, tmpFile);
        fclose(tmpFile);
#ifdef __linux__
        sprintf(command, "%s/leches %d %s tmp.%s %02x %d %d 100 %d _tmp.tap >> /dev/null", path, frequency,
                channel_type - 1 ? (channel_type - 2 ? "stereoinv" : "stereo") : "mono",
                ext, in[0], velo, offset, in[0] ? 2000 : 200);

        if (system(command))
        {
            perror("Error");
            exit(-1);
        }
#else
        sprintf(command, "%s\\leches.exe %d %s tmp.%s %02x %d %d 100 %d _tmp.tap > nul", path, frequency,
                channel_type - 1 ? (channel_type - 2 ? "stereoinv" : "stereo") : "mono",
                ext, in[0], velo, offset, in[0] ? 2000 : 200);
        if (system(command))
        {
            perror("Error");
            exit(-1);
        }
#endif
        else
        {
            sprintf(command, "tmp.%s", ext);
            if ((tmpFile = fopen(command, "rb")))
            {
                if (tzx)
                    fseek(tmpFile, 0, SEEK_END),
                        i = ftell(tmpFile) - 10,
                        fseek(tmpFile, 10, SEEK_SET);
                else
                {
                    if (fread(mem, 1, 44, tmpFile) != 44)
                        printf("Error: Reading wav header\n"),
                            exit(-1);
                    i = *(uint32_t *)(mem + 40);
                }
                while (i > 0)
                {
                    int l = fread(in, 1, 0x10000, tmpFile);
                    fwrite(in, 1, l, outFile);
                    i -= l;
                }
                fclose(tmpFile);
            }
            else
                perror(command),
                    exit(-1);
        }
        tlength -= 2 + length;
    }
    remove(command);
    remove("_tmp.tap");
    remove("nul");
    if (!tzx)
    {
        uint32_t len = ftell(outFile) - 8;
        fseek(outFile, 4, SEEK_SET);
        fwrite(&len, sizeof(len), 1, outFile);
        len -= 36;
        fseek(outFile, 40, SEEK_SET);
        fwrite(&len, sizeof(len), 1, outFile);
    }
    fclose(outFile);
}
