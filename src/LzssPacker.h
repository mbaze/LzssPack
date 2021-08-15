#ifndef LZSS_PACKER_H
#define LZSS_PACKER_H

#include <vector>
#include <memory>

bool Pack(const uint8_t* pInputStream, size_t inputSize, std::vector<uint8_t>& outputStream, bool reverseOrder);
bool Unpack(const uint8_t* pInputStream, size_t inputSize, std::vector<uint8_t>& outputStream, bool reverseOrder);

#endif // LZSS_PACKER_H
