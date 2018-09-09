#ifndef HALFSIPHASH_HPP
#define HALFSIPHASH_HPP

#include <cstdint>
#include <cstring>

/** \brief HalfSipHash algorithm returning 32-bit tags.
 *  \sa https://www.131002.net/siphash/
 *  \tparam C number of compression rounds.
 *  \tparam D number of finalization rounds.
 */
template<int C, int D>
class HalfSipHash
{
public:
  static_assert(C > 0, "C must be positive");
  static_assert(D > 0, "D must be positive");

  /** \brief Start keyed computation.
   *  \param key the 64-bit key; it's safe to read from PROGMEM directly.
   */
  void
  init(const uint8_t key[8])
  {
    m_v0 = *reinterpret_cast<const uint32_t*>(key + 0);
    m_v1 = *reinterpret_cast<const uint32_t*>(key + 4);
    m_v2 = 0x6c796765 ^ m_v0;
    m_v3 = 0x74656462 ^ m_v1;
    m_left = 0;
    m_cnt = 0;
  }

  /** \brief Append input bytes.
   *
   *  It's safe to read from PROGMEM directly if all inputs are provided at 4-byte boundary.
   */
  void
  update(const void* input, size_t len)
  {
    m_cnt += static_cast<uint8_t>(len);
    const uint8_t* in = reinterpret_cast<const uint8_t*>(input);
    if (m_left > 0) {
      while (m_left < 4 && len > 0) {
        m_block[m_left++] = *in++;
        --len;
      }
      if (m_left == 4) {
        this->append(m_block);
        m_left = 0;
      } else {
        return;
      }
    }
    while (len >= 4) {
      this->append(in);
      in += 4;
      len -= 4;
    }
    memcpy(m_block, in, len);
    m_left = len;
  }

  /** \brief Finalize computation and return hash value.
   */
  uint32_t
  final()
  {
    for (; m_left < 3; ++m_left) {
      m_block[m_left] = 0;
    }
    m_block[3] = m_cnt;
    this->append(m_block);

    m_v2 ^= 0xff;
    for (int i = 0; i < D; ++i) {
      this->sipRound();
    }
    return m_v1 ^ m_v3;
  }

private:
  void
  append(const uint8_t block[4])
  {
    uint32_t m = *reinterpret_cast<const uint32_t*>(block);
    m_v3 ^= m;
    for (int i = 0; i < C; ++i) {
      this->sipRound();
    }
    m_v0 ^= m;
  }

  void
  sipRound()
  {
    m_v0 += m_v1;
    m_v1 = rotl<5>(m_v1);
    m_v1 ^= m_v0;
    m_v0 = rotl<16>(m_v0);
    m_v2 += m_v3;
    m_v3 = rotl<8>(m_v3);
    m_v3 ^= m_v2;
    m_v0 += m_v3;
    m_v3 = rotl<7>(m_v3);
    m_v3 ^= m_v0;
    m_v2 += m_v1;
    m_v1 = rotl<13>(m_v1);
    m_v1 ^= m_v2;
    m_v2 = rotl<16>(m_v2);
  }

  template<int B>
  static uint32_t
  rotl(uint32_t x)
  {
    return (x << B) | (x >> (32 - B));
  }

private:
  uint32_t m_v0;
  uint32_t m_v1;
  uint32_t m_v2;
  uint32_t m_v3;
  uint8_t m_block[4];
  uint8_t m_left;
  uint8_t m_cnt;
};

using HalfSipHash24 = HalfSipHash<2, 4>;
using HalfSipHash13 = HalfSipHash<1, 3>;

#endif // HALFSIPHASH_HPP
