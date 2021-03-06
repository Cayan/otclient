/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "rsa.h"

Rsa::Rsa()
{
    m_keySet = false;
    mpz_init2(m_p, 1024);
    mpz_init2(m_q, 1024);
    mpz_init2(m_d, 1024);
    mpz_init2(m_u, 1024);
    mpz_init2(m_dp, 1024);
    mpz_init2(m_dq, 1024);
    mpz_init2(m_mod, 1024);
}

Rsa::~Rsa()
{
    mpz_clear(m_p);
    mpz_clear(m_q);
    mpz_clear(m_d);
    mpz_clear(m_u);
    mpz_clear(m_dp);
    mpz_clear(m_dq);
    mpz_clear(m_mod);
}

void Rsa::setKey(const char* p, const char* q, const char* d)
{
    mpz_set_str(m_p, p, 10);
    mpz_set_str(m_q, q, 10);
    mpz_set_str(m_d, d, 10);

    mpz_t pm1,qm1;
    mpz_init2(pm1,520);
    mpz_init2(qm1,520);

    mpz_sub_ui(pm1, m_p, 1);
    mpz_sub_ui(qm1, m_q, 1);
    mpz_invert(m_u, m_p, m_q);
    mpz_mod(m_dp, m_d, pm1);
    mpz_mod(m_dq, m_d, qm1);

    mpz_mul(m_mod, m_p, m_q);

    mpz_clear(pm1);
    mpz_clear(qm1);

    m_keySet = true;
}

void Rsa::encrypt(char* msg, int32_t size, const char* key)
{
    mpz_t plain, c;
    mpz_init2(plain, 1024);
    mpz_init2(c, 1024);

    mpz_t e;
    mpz_init(e);
    mpz_set_ui(e,65537);

    mpz_t mod;
    mpz_init2(mod, 1024);
    mpz_set_str(mod, key, 10);

    mpz_import(plain, 128, 1, 1, 0, 0, msg);
    mpz_powm(c, plain, e, mod);

    size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
    memset(msg, 0, 128 - count);
    mpz_export(&msg[128 - count], NULL, 1, 1, 0, 0, c);

    mpz_clear(c);
    mpz_clear(plain);
    mpz_clear(e);
    mpz_clear(mod);
}

bool Rsa::decrypt(char* msg, int32_t size)
{
    if(!m_keySet)
        return false;

    mpz_t c,v1,v2,u2,tmp;
    mpz_init2(c, 1024);
    mpz_init2(v1, 1024);
    mpz_init2(v2, 1024);
    mpz_init2(u2, 1024);
    mpz_init2(tmp, 1024);

    mpz_import(c, 128, 1, 1, 0, 0, msg);

    mpz_mod(tmp, c, m_p);
    mpz_powm(v1, tmp, m_dp, m_p);
    mpz_mod(tmp, c, m_q);
    mpz_powm(v2, tmp, m_dq, m_q);
    mpz_sub(u2, v2, v1);
    mpz_mul(tmp, u2, m_u);
    mpz_mod(u2, tmp, m_q);
    if(mpz_cmp_si(u2, 0) < 0){
            mpz_add(tmp, u2, m_q);
            mpz_set(u2, tmp);
    }
    mpz_mul(tmp, u2, m_p);
    mpz_set_ui(c, 0);
    mpz_add(c, v1, tmp);

    size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
    memset(msg, 0, 128 - count);
    mpz_export(&msg[128 - count], NULL, 1, 1, 0, 0, c);

    mpz_clear(c);
    mpz_clear(v1);
    mpz_clear(v2);
    mpz_clear(u2);
    mpz_clear(tmp);
    return true;
}
