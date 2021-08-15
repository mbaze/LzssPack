#include "LzssPacker.h"

// Slightly suboptimal brute-force search starting from every possible window offset.

void FindPhrase(const uint8_t* pInput, uint16_t maxOffset, uint16_t maxSize, uint16_t& phraseOffset, uint16_t& phraseSize)
{
    uint16_t bestOffset = 0;
    uint16_t bestSize = 0;

    for (uint16_t offset = 1; offset <= maxOffset; offset++)
    {
        const uint8_t* pPhrase = pInput - offset;

        uint16_t size = 0;
        while ((pInput[size] == pPhrase[size]) && (size < maxSize))
        {
            size++;
        }

        if (size > bestSize)
        {
            bestOffset = offset;
            bestSize = size;
        }
    }

    phraseOffset = bestOffset;
    phraseSize = bestSize;
}

void OutputPhrase(uint16_t offset, uint16_t size, std::vector<uint8_t>& outputStream)
{
    uint8_t marker0 = static_cast<uint8_t>(((size - 3) << 5) | ((offset >> 7) & 0b11110));
    uint8_t marker1 = static_cast<uint8_t>(offset & 0xFF);

    outputStream.push_back(marker0);
    outputStream.push_back(marker1);
}

void OutputLiteral(const uint8_t* pData, uint16_t size, std::vector<uint8_t>& outputStream)
{
    outputStream.push_back(2 * size - 1);
    outputStream.insert(outputStream.end(), pData, pData + size);
}

bool Pack(const uint8_t* pInputStream, size_t inputSize, std::vector<uint8_t>& outputStream, bool reverseOrder)
{
    if (pInputStream == nullptr || inputSize == 0)
    {
        return false;
    }

    std::unique_ptr<uint8_t[]> spReversedStream;

    if (reverseOrder)
    {
        spReversedStream = std::make_unique<uint8_t[]>(inputSize);
        memcpy(spReversedStream.get(), pInputStream, inputSize);
        std::reverse(spReversedStream.get(), spReversedStream.get() + inputSize);
        pInputStream = spReversedStream.get();
    }

    outputStream.clear();

    uint16_t literalSize = 0;
    const uint8_t* pCurrentPos = pInputStream;
    const uint8_t* pEndOfStream = pInputStream + inputSize;

    while (pCurrentPos < pEndOfStream)
    {
        uint16_t maxOffset = static_cast<uint16_t>(std::min(4095ll, pCurrentPos - pInputStream));
        uint16_t maxSize = static_cast<uint16_t>(std::min(10ll, pEndOfStream - pCurrentPos));

        uint16_t phraseOffset, phraseSize;
        FindPhrase(pCurrentPos, maxOffset, maxSize, phraseOffset, phraseSize);

        if (phraseSize >= 3)
        {
            if (literalSize)
            {
                OutputLiteral(pCurrentPos - literalSize, literalSize, outputStream);
                literalSize = 0;
            }

            OutputPhrase(phraseOffset, phraseSize, outputStream);
            pCurrentPos += phraseSize;
        }
        else
        {
            pCurrentPos++;
            literalSize++;

            if (literalSize >= 128)
            {
                OutputLiteral(pCurrentPos - literalSize, literalSize, outputStream);
                literalSize = 0;
            }
        }
    }

    if (literalSize)
    {
        OutputLiteral(pCurrentPos - literalSize, literalSize, outputStream);
    }

    // Add end of stream marker.

    outputStream.push_back(0);
    outputStream.push_back(0);

    if (reverseOrder)
    {
        std::reverse(outputStream.begin(), outputStream.end());
    }

    return true;
}

bool Unpack(const uint8_t* pInputStream, size_t inputSize, std::vector<uint8_t> &outputStream, bool reverseOrder)
{
    if (pInputStream == nullptr)
    {
        return false;
    }

    std::unique_ptr<uint8_t[]> spReversedStream;

    if (reverseOrder)
    {
        spReversedStream = std::make_unique<uint8_t[]>(inputSize);
        memcpy(spReversedStream.get(), pInputStream, inputSize);
        std::reverse(spReversedStream.get(), spReversedStream.get() + inputSize);
        pInputStream = spReversedStream.get();
    }

    outputStream.clear();

    while (pInputStream[0] | pInputStream[1])
    {
        if (pInputStream[0] & 1)
        {
            uint16_t size = (pInputStream[0] >> 1) + 1;
            pInputStream++;

            outputStream.insert(outputStream.end(), pInputStream, pInputStream + size);
            pInputStream += size;
        }
        else
        {
            uint16_t size = ((pInputStream[0] >> 5) & 7) + 3;
            uint16_t offset = ((pInputStream[0] << 7) & 0xF00) | pInputStream[1];

            for (uint16_t i = 0; i < size; i++)
            {
                outputStream.push_back(outputStream[outputStream.size() - offset]);
            }

            pInputStream += 2;
        }
    }

    if (reverseOrder)
    {
        std::reverse(outputStream.begin(), outputStream.end());
    }

    return true;
}
