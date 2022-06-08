#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char table1[][4] =
    {{1, 2, 2, 3}, {2, 2, 3, 3}, {2, 3, 3, 4}, {3, 3, 4, 4}, {1, 2, 3, 4}, {2, 3, 4, 5}, {2, 3, 4, 5}, {3, 4, 5, 6}};
unsigned char table2[][4] =
    {{1, 1, 2, 2}, {1, 2, 2, 3}, {2, 2, 3, 3}, {2, 3, 3, 4}, {1, 2, 3, 4}, {1, 2, 3, 4}, {2, 3, 4, 5}, {2, 3, 4, 5}};
unsigned char byvel[][8] =
    {{0xed, 0xde, 0xd2, 0xc3, 0x00, 0x71, 0x62, 0x53},
     {0xf1, 0xe5, 0xd6, 0xc7, 0x04, 0x78, 0x69, 0x5d}};
unsigned char termin[][8] =
    {{21, 22, 23, 24, 23, 24, 25, 26},
     {13, 14, 15, 16, 15, 16, 17, 18}};
unsigned char *mem, *precalc;
char *ext;
unsigned char inibit = 0, tzx = 0, channel_type = 1, checksum, mlow, velo, refconf;
FILE *inFile, *outFile;
int i, j, k, flag, ind = 0;
unsigned short length, outbyte = 1, frequency, pilotts, pilotpulses;

void outbits(short val)
{
    if (tzx)
        for (i = 0; i < val; i++)
            if (outbyte > 0xff)
                precalc[ind++] = outbyte & 0xff,
                outbyte = 2 | inibit;
            else
                outbyte <<= 1,
                    outbyte |= inibit;
    else
        for (i = 0; i < val; i++)
        {
            precalc[ind++] = inibit ? 0xc0 : 0x40;
            if (channel_type == 2)
                precalc[ind++] = inibit ? 0xc0 : 0x40;
            else if (channel_type == 6)
                precalc[ind++] = inibit ? 0x40 : 0xc0;
        }
    if (ind >= 0x10000)
        fwrite(precalc, 1, ind, outFile),
            ind = 0;
    inibit ^= 1;
}

char char2hex(char value, char *name)
{
    if (!isxdigit(value))
        printf("\nInvalid character %c or '%s' not exists\n", value, name),
            exit(-1);
    return value > '9' ? 9 + (value & 7) : value - '0';
}

int parseHex(char *name, int index)
{
    int flen = strlen(name);
    if (name[0] == '\'')
        for (i = 1; i < 11 && name[i] != '\''; ++i)
            mem[i + 6] = name[i];
    else if (~flen & 1)
    {
        flen >>= 1;
        flen > 10 && index == 7 && (flen = 10);
        for (i = 0; i < flen; i++)
            mem[i + index] = char2hex(name[i << 1 | 1], name) | char2hex(name[i << 1], name) << 4;
        ++i;
    }
    while (++i < 12)
        mem[i + 5] = ' ';
    return flen;
}

int main(int argc, char *argv[])
{
    mem = (unsigned char *)malloc(0x20000);

    if (argc != 10)
    {
        printf("\n"
               "leches v0.03, an ultra load block generator by Antonio Villena, 17 Sep 2014\n\n"
               "  leches <srate> <channel_type> <ofile> <flag> <pilot_ms> <pause_ms> <ifile>\n\n"
               "  <srate>         Sample rate, 44100 or 48000. Default is 44100\n"
               "  <channel_type>  Possible values are: mono (default), stereo or stereoinv\n"
               "  <ofile>         Output file, between TZX or WAV file\n"
               "  <flag>          Flag byte, 00 for header, ff or another for data blocks\n"
               "  <speed>         Between 0 and 7. [0..3] for Safer and [4..7] for Reckless\n"
               "  <offset>        -2,-1,0,1 or 2. Fine grain adjust for symbol offset\n"
               "  <pilot_ms>      Duration of pilot in milliseconds\n"
               "  <pause_ms>      Duration of pause after block in milliseconds\n"
               "  <ifile>         Hexadecimal string or filename as data origin of that block\n\n"),
            exit(0);
    }

    frequency = atoi(argv[1]);
    if (frequency != 44100 && frequency != 48000)
        printf("\nInvalid sample rate: %d\n", frequency),
            exit(-1);

    if (!(ext = strrchr(argv[3], '.')))
        printf("%s: Output file name must have .wav or .tzx extension\n", argv[3]),
            exit(-1);

    if (!strcasecmp(argv[2], "mono"))
        channel_type = 1;
    else if (!strcasecmp(argv[2], "stereo"))
        channel_type = 2;
    else if (!strcasecmp(argv[2], "stereoinv"))
        channel_type = 6;
    else
        printf("\nInvalid argument name: %s\n", argv[2]),
            exit(-1);

    if ((outFile = fopen(argv[3], "wb+")) == NULL)
        printf("\nCannot create output file: %s\n", argv[3]),
            exit(-1);

    precalc = (unsigned char *)malloc(0x200000);
    if (!strcasecmp(ext, ".tzx"))
        tzx = 1;
    else if (!strcasecmp(ext, ".wav"))
    {
        memset(mem, 0, 44);
        memset(precalc, 128, 0x200000);
        *(uint32_t *)mem = 0x46464952;
        *(uint32_t *)(mem + 8) = 0x45564157;
        *(uint32_t *)(mem + 12) = 0x20746d66;
        *(uint32_t *)(mem + 16) = 0x10;
        *(uint32_t *)(mem + 20) = 0x01;
        *(uint8_t *)(mem + 22) = *(uint8_t *)(mem + 32) = channel_type & 3;
        *(uint16_t *)(mem + 24) = frequency;
        *(uint32_t *)(mem + 28) = frequency * (channel_type & 3);
        *(uint8_t *)(mem + 34) = 8;
        *(uint32_t *)(mem + 36) = 0x61746164;
        fwrite(mem, 1, 44, outFile);
    }
    else
        printf("Output format not allowed, use only TZX or WAV\n"),
            exit(-1);
    mlow = frequency == 48000 ? 1 : 0;
    pilotts = mlow ? 875 : 952;
    pilotpulses = atof(argv[7]) * 3500 / pilotts + 0.5;
    inFile = fopen(argv[9], "rb");
    if (inFile)
        length = fread(mem, 1, 0x20000, inFile);
    else
        length = parseHex(argv[9], 0);
    velo = atoi(argv[5]);
    refconf = (byvel[mlow][velo] & 128) + ((byvel[mlow][velo] + 3 * atoi(argv[6])) & 127);
    for (checksum = i = 0; i < length; i++)
        checksum ^= mem[i];
    if (tzx)
        fprintf(outFile, "ZXTape!"),
            *(uint32_t *)precalc = 0xa011a,
                  precalc[3] = 0x12,
                  *(uint16_t *)(precalc + 4) = pilotts,
                  *(uint16_t *)(precalc + 6) = pilotpulses,
                  precalc[8] = 0x15,
                  *(uint16_t *)(precalc + 9) = mlow ? 73 : 79,
                  *(uint16_t *)(precalc + 11) = atof(argv[8]),
                  ind = 17;
    else
        while (pilotpulses--)
            outbits(12);
    outbits(28);
    pilotpulses = 6;
    while (pilotpulses--)
        outbits(12);
    outbits(2);
    outbits(mlow ? 4 : 8);
    flag = refconf | strtol(argv[4], NULL, 16) << 8 | checksum << 16;
    for (j = 0; j < 24; j++, flag <<= 1)
        outbits(k = flag & 0x800000 ? 4 : 8),
            outbits(k);
    outbits(2);
    outbits(3);
    --mem;
    while (length--)
        outbits(table1[velo][*++mem >> 6]),
            outbits(table2[velo][*mem >> 6]),
            outbits(table1[velo][*mem >> 4 & 3]),
            outbits(table2[velo][*mem >> 4 & 3]),
            outbits(table1[velo][*mem >> 2 & 3]),
            outbits(table2[velo][*mem >> 2 & 3]),
            outbits(table1[velo][*mem & 3]),
            outbits(table2[velo][*mem & 3]);
    outbits(termin[mlow][velo] >> 1);
    outbits(termin[mlow][velo] - (termin[mlow][velo] >> 1));
    outbits(1);
    outbits(1);
    if (tzx)
    {
        for (j = 8; outbyte < 0x100; outbyte <<= 1, --j)
            ;
        precalc[ind++] = outbyte;
        fwrite(precalc, 1, ind, outFile);
        i = j | (ftell(outFile) - 24) << 8;
        fseek(outFile, 20, SEEK_SET);
        fwrite(&i, 4, 1, outFile);
    }
    else
    {
        fwrite(precalc, 1, ind, outFile);
        fwrite(precalc + 0x100000, 1, frequency * (channel_type & 3) * atof(argv[8]) / 1000, outFile);
        uint32_t len = ftell(outFile) - 8;
        fseek(outFile, 4, SEEK_SET),
            fwrite(&len, sizeof(len), 1, outFile),
            len -= 36,
            fseek(outFile, 40, SEEK_SET),
            fwrite(&len, sizeof(len), 1, outFile);
    }
    fclose(inFile);
    fclose(outFile);
    printf("\nFile generated successfully\n");
    exit(0);
}