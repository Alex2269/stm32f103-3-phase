/***
* Table representation of half wave of sinusoid with addition of one fourth of the 3rd harmonic of sinus
* Table contains 128 values from 0 to 115 and decrease from harmonic to 97
***/

#ifndef __SIN256_H
#define __SIN256_H

#ifdef __cplusplus
 extern "C" {
#endif

unsigned char sinus[128]=
{
  0,
  6,
  11,
  17,
  22,
  27,
  33,
  38,
  43,
  48,
  53,
  58,
  62,
  67,
  71,
  75,
  79,
  83,
  86,
  90,
  93,
  96,
  99,
  101,
  103,
  105,
  107,
  109,
  110,
  112,
  113,
  113,
  114,
  114,
  115,
  115,
  115,
  115,
  114,
  114,
  114,
  113,
  112,
  111,
  111,
  110,
  109,
  108,
  107,
  106,
  105,
  104,
  103,
  102,
  101,
  100,
  100,
  99,
  98,
  98,
  98,
  97,
  97,
  97,
  97,
  97,
  97,
  97,
  98,
  98,
  98,
  99,
  100,
  100,
  101,
  102,
  103,
  104,
  105,
  106,
  107,
  108,
  109,
  110,
  111,
  111,
  112,
  113,
  114,
  114,
  114,
  115,
  115,
  115,
  115,
  114,
  114,
  113,
  113,
  112,
  110,
  109,
  107,
  105,
  103,
  101,
  99,
  96,
  93,
  90,
  86,
  83,
  79,
  75,
  71,
  67,
  62,
  58,
  53,
  48,
  43,
  38,
  33,
  27,
  22,
  17,
  11,
  6
};

#ifdef __cplusplus
}
#endif

#endif /* __SIN256_H */
