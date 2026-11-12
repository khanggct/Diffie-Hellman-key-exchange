#include "big_int.h"

uint512_t add_mod(uint512_t, uint512_t, uint512_t);
uint512_t mul_mod(uint512_t, uint512_t, uint512_t);
uint512_t pow_mod(uint512_t, uint512_t, uint512_t);
uint512_t add_mod_fast(uint512_t&, uint512_t&, uint512_t&);
uint512_t generate_safe_prime();
uint512_t generate_private_key();

uint512_t add_mod(uint512_t a, uint512_t b, uint512_t p)
{
    // a = a % p;
    // b = b % p;
    if (a >= p - b)
        return a - (p - b);
    else
        return a + b;
}

uint512_t add_mod_fast(uint512_t& a, uint512_t& b, uint512_t& p)
{
    if (a >= p - b)
        return a - (p - b);
    else
        return a + b;
}

uint512_t mul_mod(uint512_t a, uint512_t b, uint512_t p)
{
    a = a % p;
    b = b % p;
    uint512_t res;
    while (b > 0) 
    {
        if (b.is_odd())
            res = add_mod_fast(res, a, p);
        a = add_mod(a, a, p);
        b >>= 1;
    }    a = a % p;
    b = b % p;
    return res;
}

uint512_t pow_mod(uint512_t a, uint512_t e, uint512_t p)
{
    a = a % p;
    uint512_t res(1);
    while (e > 0)
    {
        if (e.is_odd())
            res = mul_mod(res, a, p);
        a = mul_mod(a, a, p);
        e >>= 1;
    }
    return res;
}

int main ()
{
    uint512_t a("4177552661081475683599851810420534221418910789510794609343985122136857843797208320966455522255100110158500179536042930353687472090934686125685536063125449");
    uint512_t b("3844670446586325551262243819209921765533632933745095454160881336449581323410756379854133680743792815441798503630756611646223247554952214736578686618948380");
    uint512_t p("4930692352244992027984528732483040530885468290520255440101412631598589248293784565510876720876402420738630272325790911918799626253465916317893308955963911");
    // uint512_t add_res = add_mod(a,b,p);
    // uint512_t mul_res = mul_mod(a,b,p);
    uint512_t pow_res = pow_mod(a, b, p);
    // add_res.print_dec();
    // mul_res.print_dec();
    pow_res.print_dec();
    return 0;
}