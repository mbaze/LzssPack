#include <stdio.h>
#include "LzssPacker.h"

int main(int argc, char** argv)
{
    bool printUsage = false;
    bool reverseOrder = false;

    if (argc == 4 && strcmp(argv[3], "-r") == 0)
    {
        reverseOrder = true;
    }
    else if (argc != 3)
    {
        printUsage = true;
    }

    if (printUsage)
    {
        printf("Usage: LzssPack.exe <input.bin> <output.lzss> [-r]\n");
        return 0;
    }

    const char* pReadError = "Error: Cannot read input file.\n";

    FILE* pInputFile;
    if (fopen_s(&pInputFile, argv[1], "rb"))
    {
        printf(pReadError);
        return 0;
    }

    if (fseek(pInputFile, 0, SEEK_END))
    {
        fclose(pInputFile);
        printf(pReadError);
        return 0;
    }

    size_t inputFileSize = ftell(pInputFile);
    rewind(pInputFile);

    std::unique_ptr<uint8_t[]> spInputStream = std::make_unique<uint8_t[]>(inputFileSize);
    if (fread(spInputStream.get(), 1, inputFileSize, pInputFile) != inputFileSize)
    {
        fclose(pInputFile);
        printf(pReadError);
        return 0;
    }

    std::vector<uint8_t> outputStream;
    if (Pack(spInputStream.get(), inputFileSize, outputStream, reverseOrder) == false)
    {
        printf("Error: Invalid stream.\n");
        return 0;
    }

    // Verify output stream's integrity.

    const char* pVerifyError = "Error: Verification failed.\n";

    std::vector<uint8_t> verifyStream;
    if (Unpack(outputStream.data(), outputStream.size(), verifyStream, reverseOrder) == false)
    {
        printf(pVerifyError);
        return 0;
    }

    if (inputFileSize != verifyStream.size())
    {
        printf(pVerifyError);
        return 0;
    }

    if (memcmp(spInputStream.get(), verifyStream.data(), inputFileSize))
    {
        printf(pVerifyError);
        return 0;
    }

    // Write output.

    const char* pWriteError = "Error: Cannot write output file.\n";

    FILE* pOutputFile;
    if (fopen_s(&pOutputFile, argv[2], "wb"))
    {
        printf(pWriteError);
        return 0;
    }

    if (fwrite(outputStream.data(), 1, outputStream.size(), pOutputFile) != outputStream.size())
    {
        fclose(pOutputFile);
        printf(pWriteError);
        return 0;
    }

    fclose(pOutputFile);

    return 0;
}
