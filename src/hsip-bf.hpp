#ifndef HSIP_BF_HPP
#define HSIP_BF_HPP

#include "bloom-filter.hpp"
#include "halfsiphash.hpp"

#ifdef ARDUINO
#include <Arduino.h>
#endif

/** \brief Bloom filter using HalfSipHash.
 *  \tparam M Bloom filter size in bits, must be power of 2.
 *  \tparam K number of hash functions.
 *  \tparam H a variant of HalfSipHash.
 */
template<size_t M, int K, typename H = HalfSipHash24>
class HsipBf : public BloomFilter<M, K>
{
public:
  /** \brief Constructor.
   *  \param keys a vector of 64-bit keys; caller must retain memory;
   *              it's safe to read from PROGMEM directly.
   */
  explicit
  HsipBf(const uint8_t keys[K * 8])
    : m_keys(keys)
  {
  }

  /** \brief Add a key.
   */
  void
  add(const void* input, size_t size)
  {
    for (int i = 0; i < K; ++i) {
      H h;
      h.init(m_keys + i * 8);
      h.update(input, size);
      this->addHash(h.final());
    }
  }

  /** \brief Test whether BF contains a key.
   */
  bool
  contains(const void* input, size_t size) const
  {
    for (int i = 0; i < K; ++i) {
      H h;
      h.init(m_keys + i * 8);
      h.update(input, size);
      if (!this->hasHash(h.final())) {
        return false;
      }
    }
    return true;
  }

#ifdef ARDUINO
  /** \brief Add a string key.
   */
  void
  add(const String& str)
  {
    this->add(str.c_str(), str.length());
  }

  /** \brief Test whether BF contains a string key.
   */
  bool
  contains(const String& str) const
  {
    return this->contains(str.c_str(), str.length());
  }
#endif

private:
  const uint8_t* m_keys;
};

#endif // HSIP_BF_HPP
