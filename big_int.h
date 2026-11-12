#ifndef BIG_INT_H
#define BIG_INT_H

#include <cstdio>
#include <stdint.h>
#include <string>
#include <cstring>
#include <stdexcept>

class uint512_t
{
private: 
    uint64_t parts[8];
public:
    uint512_t ();
    uint512_t (uint64_t);
    uint512_t (const std::string &);
    bool operator == (const uint512_t& other) const;
    bool operator < (const uint512_t& other) const;
    bool operator > (const uint512_t& other) const;
    bool operator >= (const uint512_t& other) const;
    bool operator <= (const uint512_t& other) const;
    uint512_t& operator = (const uint512_t& other);
    uint512_t operator + (const uint512_t& other) const;
    uint512_t operator - (const uint512_t& other) const;
    uint512_t operator * (const uint512_t& other) const;
    uint512_t operator / (const uint512_t& other) const;
    uint512_t operator % (const uint512_t& other) const;
    uint512_t operator / (const uint64_t) const;
    uint64_t operator % (const uint64_t) const;
    uint512_t& operator >>= (const uint8_t);
    uint64_t operator & (const uint64_t) const;

    bool is_odd();
    void print_hex ();
    void print_dec ();
};

#endif