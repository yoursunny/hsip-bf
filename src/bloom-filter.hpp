#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

#include <bitset>

/** \brief Simple Bloom filter, agnostic to hash functions.
 *  \tparam M Bloom filter size in bits, must be power of 2.
 *  \tparam K number of hash functions.
 */
template<size_t M, int K>
class BloomFilter
{
public:
  static_assert((M & (M - 1)) == 0, "M must be power of 2");
  static_assert(K > 0, "K must be positive");

  /** \brief Add a key.
   *  \param hashes a vector of hashes computed from the key.
   */
  void
  add(const size_t hashes[K])
  {
    for (int i = 0; i < K; ++i) {
      this->addHash(hashes[i]);
    }
  }

  /** \brief Clear the Bloom filter.
   */
  void
  clear()
  {
    m_f.reset();
  }

  /** \brief Test whether BF contains a key.
   *  \param hashes a vector of hashes computed from the key.
   */
  bool
  contains(const size_t hashes[K]) const
  {
    for (int i = 0; i < K; ++i) {
      if (!this->hasHash(hashes[i])) {
        return false;
      }
    }
    return true;
  }

protected:
  void
  addHash(size_t hash)
  {
    m_f.set(hash & (M - 1));
  }

  bool
  hasHash(size_t hash) const
  {
    return m_f.test(hash & (M - 1));
  }

private:
  std::bitset<M> m_f;
};

#endif // BLOOM_FILTER_HPP
