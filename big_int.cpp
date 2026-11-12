#include "big_int.h"

// Constructor
uint512_t::uint512_t ()
{
    std::memset(this->parts, 0, sizeof(this->parts));
}

uint512_t::uint512_t (uint64_t num)
{
    std::memset(this->parts, 0, sizeof(this->parts));
    this->parts[0] = num;
}

uint512_t::uint512_t (const std::string &str)
{
    std::memset(this->parts, 0, sizeof(this->parts));
    for (char c : str) 
    {
        
        if (c < '0' || c > '9') throw std::invalid_argument("received non-number value");
        uint8_t digit = c - '0';
        __uint128_t carry = 0;

        // dịch số sang trái theo hệ 10 (*10)
        for (int i = 0; i < 8; i++) 
        {
            __uint128_t prod = (__uint128_t)(this->parts[i]) * 10 + carry;
            this->parts[i] = (uint64_t)prod;
            carry = prod >> 64;
        }
        if (carry > 0) throw std::overflow_error("the number exceed 512 bits");
        
        // thêm digit mới vào cuối số
        __uint128_t sum = (__uint128_t)this->parts[0] + digit;
        this->parts[0] = (uint64_t)sum;
        carry = sum >> 64;

        int idx = 1;
        while (carry > 0 && idx < 8)
        {
            sum = (__uint128_t)this->parts[idx] + carry;
            this->parts[idx] = (uint64_t)sum;
            carry = sum >> 64;
            idx++;
        }
        if (carry > 0) throw std::overflow_error("the number exceed 512 bits");
    }
}

bool is_zero(const uint64_t parts[8])
{
    for (int i = 7; i >= 0; i--)
        if (parts[i] != 0)
            return false;
    
    return true;
}

uint64_t divide_by_10 (uint64_t parts[8])
{
    __uint128_t rem = 0;
    for (int i = 7; i >= 0; i--)
    {
        __uint128_t current = (rem << 64) | parts[i];
        parts[i] = (uint64_t)(current / 10);
        rem = current % 10;
    }
    return uint64_t(rem);
}

// void uint512_t::print_hex ()
// {
//     std::printf("0x");
//     if (is_zero(this->parts))
//     {
//         std::printf("0");
//         return;
//     }
//     for (int i = 7; i >= 0; i--)
//         if (this->parts[i] != 0)
//             std::printf("%lx", this->parts[i]);
// }



void uint512_t::print_dec ()
{
    if (is_zero(this->parts))
    {
        std::printf("0");
        return;
    }

    uint64_t temp_parts[8];
    std::memcpy(temp_parts, this->parts, sizeof(this->parts));

    std::string str = "";
    while (is_zero(temp_parts) == 0)
    {
        uint64_t rem = divide_by_10(temp_parts);
        str = (char)('0' + rem) + str;
    }
    std::printf("%s", str.c_str());
}

bool uint512_t::operator == (const uint512_t& other) const
{
    for (int i = 0; i < 8; i++)
        if (this->parts[i] != other.parts[i])
            return false;

    return true;
}

bool uint512_t::operator < (const uint512_t& other) const
{
    for (int i = 7; i >= 0; i--)
        if (this->parts[i] < other.parts[i])
            return true;
        else if (this->parts[i] > other.parts[i])
            return false;
    
    return false;
}

bool uint512_t::operator > (const uint512_t& other) const
{
    for (int i = 7; i >= 0; i--)
        if (this->parts[i] > other.parts[i])
            return true;
        else if (this->parts[i] < other.parts[i])
            return false;
    
    return false;
}

bool uint512_t::operator >= (const uint512_t& other) const
{
    return *this > other || *this == other;
}

bool uint512_t::operator <= (const uint512_t& other) const
{
    return *this < other || *this == other;
}

uint512_t& uint512_t::operator = (const uint512_t& other)
{
    if (this != &other)
    {
        std::memcpy(this->parts, other.parts, sizeof(other.parts));
    }
    return *this;
}

uint512_t uint512_t::operator + (const uint512_t& other) const
{
    __uint128_t carry = 0;
    uint512_t res;
    for (int i = 0; i < 8; i++) 
    {
        __uint128_t sum = (__uint128_t)this->parts[i] + (__uint128_t)other.parts[i] + carry;
        res.parts[i] = (uint64_t)sum;
        carry = sum >> 64;
    }
    if (carry > 0) throw std::overflow_error("addition overflow");
    return res;
}

uint512_t uint512_t::operator - (const uint512_t& other) const
{
    if (*this < other) throw std::underflow_error("subtraction underflow");
    bool borrow = 0;
    uint512_t res;
    for (int i = 0; i < 8; i++)
    {
        __uint128_t diff = (__uint128_t)this->parts[i] - other.parts[i] - borrow;
        res.parts[i] = (uint64_t)diff;
        borrow = (diff >> 64) ? 1 : 0;
    }
    return res;
}

uint512_t uint512_t::operator * (const uint512_t& other) const
{
    uint64_t tmp[16] = {0};
    for (int i = 0; i < 8; i++)
    {
        __uint128_t carry = 0;
        for (int j = 0; j < 8; j++)
        {
            __uint128_t cur = (__uint128_t)this->parts[i] * other.parts[j] + tmp[i+j] + carry;
            tmp[i+j] = (uint64_t)cur;
            carry = cur >> 64;
        }
        tmp[i+8] += (uint64_t)carry;
    }

    for (int i = 8; i < 16; i++)
        if (tmp[i] > 0) throw std::overflow_error("multiplication overflow");
    
    uint512_t res;
    std::memcpy(res.parts, tmp, sizeof(res.parts));
    return res;
}



uint512_t uint512_t::operator / (const uint512_t& other) const
{
    if (is_zero(other.parts)) 
        throw std::domain_error("division by zero");

    uint512_t q, r;

    for (int i = 511; i >= 0; i--)
    {
        uint64_t carry_out = r.parts[7] >> 63;
        for (int k = 7; k > 0; k--)
            r.parts[k] = (r.parts[k] << 1) | (r.parts[k-1] >> 63);
        r.parts[0] <<= 1;
        r.parts[0] |= this->parts[i/64] >> (i % 64) & 1;

        if (carry_out || r > other || r == other)
        {
            r = r - other;
            q.parts[i/64] |= (1ULL << (i % 64));
        }
    }
    return q;
}

uint512_t uint512_t::operator % (const uint512_t& other) const
{
    if (is_zero(other.parts)) 
        throw std::domain_error("modular by zero");

    uint512_t q, r;

    for (int i = 511; i >= 0; i--)
    {
        uint64_t carry_out = r.parts[7] >> 63;
        for (int k = 7; k > 0; k--)
            r.parts[k] = (r.parts[k] << 1) | (r.parts[k-1] >> 63);
        r.parts[0] <<= 1;
        r.parts[0] |= this->parts[i/64] >> (i % 64) & 1;

        if (carry_out || r > other || r == other)
        {
            r = r - other;
            q.parts[i/64] |= (1ULL << (i % 64));
        }
    }
    return r;
}

uint64_t uint512_t::operator % (const uint64_t a) const
{
    if (a == 0) 
        throw std::domain_error("modular by zero");

    __uint128_t rem = 0;
    uint512_t temp = *this;
    for (int i = 7; i >= 0; i--)
    {
        __uint128_t current = (rem << 64) | temp.parts[i];
        temp.parts[i] = (uint64_t)(current / a);
        rem = current % a;
    }
    return uint64_t(rem);
}

uint512_t uint512_t::operator / (const uint64_t a) const
{
    if (a == 0) 
            throw std::domain_error("division by zero");

    __uint128_t rem = 0;
    uint512_t temp = *this;
    for (int i = 7; i >= 0; i--)
    {
        __uint128_t current = (rem << 64) | temp.parts[i];
        temp.parts[i] = (uint64_t)(current / a);
        rem = current % a;
    }
    return temp;
}

uint512_t& uint512_t::operator >>= (const uint8_t a)
{
    if (a > 64) throw std::underflow_error("shift-right overflow");
    for (int i = 0; i < 7; i++)
        this->parts[i] = (this->parts[i] >> a) | (this->parts[i+1] << (64-a));
    this->parts[7] >>= a;
    return *this;
}

bool uint512_t::is_odd()
{
    return this->parts[0] & 1;
}