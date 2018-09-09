# HalfSipHash and Bloom Filter for Arduino

`halfsiphash.hpp` implements **HalfSipHash** algorithm.
It is a variant of [SipHash: a fast short-input PRF](https://131002.net/siphash/).
HalfSipHash is efficient on 32-bit microcontrollers such as Xtensa.

`hsip-bf.hpp` implements a **Bloom filter** data structure using HalfSipHash algorithm.
Bloom filter size and number of hash functions are both configurable at compile time.
HalfSipHash keys can reside in PROGMEM.
