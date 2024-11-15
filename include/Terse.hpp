//
//  Terse.hpp
//  Terse
//
//  Created by Jan Pieter Abrahams on 30/04/2019.
//  Copyright © 2019 Jan Pieter Abrahams. All rights reserved.
//

#ifndef Terse_h
#define Terse_h

#include <fstream>
#include <vector>
#include <charconv>
#include <cassert>
#include <cmath>
#include "Bit_pointer.hpp"
#include "XML_element.hpp"

// Terse<T> allows efficient and fast compression of integral diffraction data and other integral greyscale
// data into a Terse object that can be decoded by the member function Terse<T>::prolix(iterator). The
// prolix(iterator) member function decompresses the data starting at the location defined by 'iterator'
// (which can also be a pointer). A Terse object is constructed by supplying it with uncompressed data or a
// stream that contains compressed Terse data.
//
// A Terse object may contain compressed data of multiple frames, and data of a particular frame can be
// extracted by indexing. All frames must have the same size and dimensions, and must all be signed or unsigned.
//
// A Terse object can be unpacked into any arithmetic type, including float and double. Unpacking into
// values of type T with fewer bits than the original data, results in truncation of overflowed data to
// std::numeric_limits<T>::max(),and to std::numeric_limits<T>::min() for underflowed signed types.
// Unpacking signed data into unsigned values is not allowed. Compressing as unsigned yields a tighter
// compression.
//
// A Terse object can be written or appended to any stream. The resulting file is independent of the endian-nes
// of the machine: both big- and small-endian machines produce identical files, making data transfer optimally
// transparent.
//
// Terse data in a file are immediately preceded by a small header, which is encoded in standard XML as follows:
// <Terse prolix_bits="n" signed="s" block="b" memory_size="m" number_of_values="v" [dimensions="d [...]"] [number_of_frams="f"]/>
//   - "n" is the number of bits required for the most extreme value in the Terse data
//   - "s" is "0" for unsigned data, "1" for signed data
//   - "b" is the block size of the stretches of data values that are encoded (by default 12 values)
//   - "m" is the number of bytes of Terse data, excluding the header, but including all frames in an encoded stack
//   - "v" is the number of values of a single frame of a stack
//   - "d [...]" is optional. It encodes the dimensions of a single frame. Frames can have any number of dimensions.
//   - "f" is optional. if it is absent, a single frame is encoded, if it is present, f indicates the number of frames.
// Here is an example:
// <Terse prolix_bits="12" signed="0" block="12" memory_size="91388" number_of_values="262144" dimensions="512 512" number_of_frames="2"/>
//
// The algorithm is a run-length encoding type. Each data block (by default 12 integral values in the
// constructor, but this can be changed) is preceded by one or more data block header bits. The values in the
// data block are stripped of their most significant bits, provided they are all zero (for unsigned values),
// or either all zero or one (for signed values). In the latter case, the sign bit is maintained. So, for a
// block size of 3 with values 3, 4, 2, the encoded bits would be: 011 (denoting 3) 100 (denoting 4) 010
// (denoting 2). So 011100010 would be pushed into the Terse object. In case of signed values -3, 4, 2, the
// encoded bits would be 1011 (denoting -3) 0100 (denoting +4) 0010 (denoting +2), resulting in a data block
// 101101000010. So if the values that need to be encoded are all positive or zero, they should be encoded as
// unsigned for optimal compression: it saves 1 bit per encoded value.
//
// The header bits define how the values are encoded. They have the following following structure:
// bit 1:    If the first bit of the block header is set, then there are no more bits in the block header,
//           and the parameters of the previous block header are used.
// bit 2-4:  The first header bit is 0. The three bits 2 to 4 define how many bits are used per value of the
//           encoded block. If bits 2 to 4 are all set, 7 or more bits per value are required and the
//           header is expanded by a further 2 bits.
// bit 5-6:  The first 4 header bits are 0111. The number encoded bits 5 and 6 is added to 7 and this defines
//           how many bits are used to encode the block. So if bits 5 & 6 are 00 then 7 bits are used, 01 means
//           8 bits, 10 means 9 bits and 11 means at least 10 bits. If bits 5 & 6 are both set, the header is
//           expanded by another 6 bits.
// bit 7-12: The first 6 header bits are 011111. The number encoded by bits 7 to 12 is added to decimal 10 and
//           this defines how many bits are used to encode the block. So if bits 7 to 12 are 000000, then the
//           number of bits per value in the data block is decimal 10. If bits 7 to 12 are 110110, then the
//           number of bits per value in the data block is 10 + 54 = 64.
//
// Constructors:
//  Terse(std::ifstream& istream)
//      Reads in a Terse object that has been written to a file by the overloaded Terse output operator '<<'.
//  Terse(container_type const& data)
//      Creates a Terse object from data (which can be a std::vector, Field, etc.). Only containers of
//      integral types are allowed. If the container has a member function dim(), that will set the dimensions
//      of the Terse object. Otherwise the dimensions can be set once using the dim(vector const&) member function.
//  Terse(iterator begin, std::size_t size)
//      Creates a Terse object given a starting iterator or pointer and the number of elements that need to be
//      encoded.
//
// Member functions:
//  std::size_t size()
//      Returns the number of encoded elements.
//  bool is_signed()
//      Returns true if the encoded data are signed, false if unsigned. Signed data cannot be decompressed into
//      unsigned data.
//  bits_per_val()
//      Returns the maximum number of bits per element that can be expected. So for uncompressed uint_16 type
//      data, bits_per_val() returns 16. Terse data cannot be decompressed into a container type with elements
//      that are smaller in bits than bits_per_val().
//  terse_size()
//      Returns the number of bytes used for encoding the Terse data.
//  void push_back(Iterator const begin, size_t const size)
//  void push_back(container_type const& container)
//      Adds another frame to the Terse object. The new frame is defined by its begin iterator and size, or by a
//      reference to a container. The size must be the same as that of the frame used to create the Terse object.
//      If the container has a member function dim(), that must return the same dimension as provided for the first frame.
//  std::size_t const number_of_frames() const
//      Returns the number of frames stored in the Terse object
//  std::vector<std::size_t> const& dim() const
//      Returns the dimensions of each of the Terse frames (all frames must have the same dimensions).
//  std::vector<std::size_t> const& dim(std::vector<std::size_t> const& dim) {
//      Sets the dimensions of the Terse frames. Since all frames must have the same dimensions, they can be set only once.
//  void prolix(iterator begin)
//      Unpacks the Terse data, storing it from the location defined by 'begin'. Terse integral signed data cannot be
//      unpacked into integral unsigned data. Terse data cannot be decompressed into elements that are smaller
//      in bits than bits_per_val(), but can be decompressed into larger values. Terse data can always be unpacked
//      into signed intergral, double and float data and will have the correct sign (with one exception: an
//      unsigned overflowed - all 1's - value will be unpacked as -1 signed value. As all other values are positive
//      in this case, such a situation is easy to recognise).
//  void prolix(container_type& container)
//      Unpacks the Terse data and stores it in the provided container. Also checks the container is large enough.
//  void write(Streamtype &ostream)
//      Writes Terse data to 'ostream'. The Terse data are preceded by an XML element containing the parameters
//      that are required for constructing a Terse object from the stream. Data are written as a byte stream
//      and are therefore independent of endian-ness. A small-endian memory lay-out produces the a Terse file
//      that is identical to a big-endian machine.
//
//
// Example:
//
//    std::vector<int> numbers(1000);                   // Uncompressed data location
//    std::iota(numbers.begin(), numbers.end(), -500);  // Fill with numbers -500, -499, ..., 499
//    Terse trpx(numbers);                        // Compress the data to less than 30% of memory
//    std::cout << "compression rate " << float(compressed.terse_size()) / (numbers.size() * sizeof(unsigned)) << std::endl;
//    std::ofstream outfile("junk.trpx");
//    compressed.write(outfile);                        // Write Terse data to disk
//    std::ifstream infile("junk.trpx");
//    Terse from_file(infile);                          // Read it back in again
//    std::vector<int> uncompressed(1000);
//    from_file.prolix(uncompressed.begin());           // Decompress the data...
//    for (int i=0; i != 5; ++i)
//    std::cout << uncompressed[i] << std::endl;
//    for (int i=995; i != 1000; ++i)
//    std::cout << uncompressed[i] << std::endl;
//
// Produces as output:
//
//    compression rate 0.29
//    -500
//    -499
//    -498
//    -497
//    -496
//    495
//    496
//    497
//    498
//    499


namespace jpa {

/**
 * @class Terse
 * @brief Terse<T> allows efficient and fast compression of integral diffraction data and other integral greyscale data.
 *
 * The Terse class is used for compression and decompression of integral diffraction data and other integral greyscale data.
 * It supports efficient compression and decompression, as well as data extraction and output to streams.
 *
 * A Terse object may contain compressed data of multiple frames, and data of a particular frame can be
 * extracted by indexing. All frames in a Terse object must have the same size and dimensions, and must all be signed or unsigned.
 *
 * A Terse object can be unpacked into any arithmetic type, including float and double. Unpacking into
 * values of type T with fewer bits than the original data results in truncation of overflowed data to
 * std::numeric_limits<T>::max() and to std::numeric_limits<T>::min() for underflowed signed types.
 * Unpacking signed data into unsigned values is not allowed. Compressing as unsigned yields a tighter compression.
 *
 * A Terse object can be written or appended to any stream. The Terse data in the file is independent of the endian-ness
 * of the machine: both big-endian and small-endian machines produce identical files, making data transfer optimally
 * transparent.
 *
 * Terse data in a file are immediately preceded by a small header, which is encoded in standard XML as follows:
 * <pre>
 * <Terse prolix_bits="n" signed="s" block="b" memory_size="m" number_of_values="v" [dimensions="d [...]"] [number_of_frames="f"]/>
 * </pre>
 *   - "n" is the number of bits required for the most extreme value in the Terse data.
 *   - "s" is "0" for unsigned data, "1" for signed data.
 *   - "b" is the block size of the stretches of data values that are encoded (by default 12 values).
 *   - "m" is the number of bytes of Terse data, excluding the header, but including all frames in an encoded stack.
 *   - "v" is the number of values of a single frame of a stack.
 *   - "d [...]" is optional. It encodes the dimensions of a single frame. Frames can have any number of dimensions.
 *   - "f" is optional. If it is absent, a single frame is encoded; if it is present, "f" indicates the number of frames.
 *
 * Here is an example of a Terse file with two frames of 512x512 pixels:
 * <pre>
 * <Terse prolix_bits="12" signed="0" block="12" memory_size="91388" number_of_values="262144" dimensions="512 512" number_of_frames="2"/>
 * </pre>
 *
 * Example of usage:
 * \code{.cpp}
 *    std::vector<int> numbers(1000);                   // Uncompressed data location
 *    std::iota(numbers.begin(), numbers.end(), -500);  // Fill with numbers -500, -499, ..., 499
 *    Terse trpx(numbers);                              // Compress the data to less than 30% of memory
 *    std::cout << "compression rate " << float(compressed.terse_size()) / (numbers.size() * sizeof(unsigned)) << std::endl;
 *    std::ofstream outfile("junk.trpx");
 *    compressed.write(outfile);                        // Write Terse data to disk
 *    std::ifstream infile("junk.trpx");
 *    Terse from_file(infile);                          // Read it back in again
 *    std::vector<int> uncompressed(1000);
 *    from_file.prolix(uncompressed.begin());           // Decompress the data...
 *    for (int i = 0; i != 5; ++i)
 *        std::cout << uncompressed[i] << std::endl;
 *    for (int i = 995; i != 1000; ++i)
 *        std::cout << uncompressed[i] << std::endl;
 * \endcode
 *
 * Produces the following output:
 * <pre>
 *    compression rate 0.29
 *    -500
 *    -499
 *    -498
 *    -497
 *    -496
 *    495
 *    496
 *    497
 *    498
 *    499
 * </pre>
 */
class Terse {
public:
    
    /**
     * @brief Initializes an empty Terse object.
     *
     * Data can be appended to an empty Terse object. The first dataset to be pushed in
     * determines size and signedness of the remaining datasets that can be pushed in.
     */
    Terse(){};
    
    /**
     * @brief Creates a Terse object from data (which can be a std::vector, Field, etc.).
     *
     * Only containers of integral types are allowed. If the container has a member function dim(),
     * that will set the dimensions of the Terse object. Otherwise, the dimensions can be set once
     * using the dim(vector const&) member function.
     *
     * @tparam Container The type of the container containing integral data.
     * @param data The container containing integral data.
     */
    template <typename Container> requires (requires (Container c) {c.begin(); c.size();})
    Terse(Container const& data) : Terse(data.begin(), data.size()) {
        if constexpr(requires (Container &c) {c.dim();})
            for (auto d : data.dim()) d_dim.push_back(d);
    };
    
    /**
     * @brief Creates a Terse object given a starting iterator or pointer and the number of elements that need to be encoded.
     *
     * @tparam Iterator The type of the iterator.
     * @param data The starting iterator or pointer to the data.
     * @param size The number of elements to be encoded.
     * @param block The block size for compression (default is 12).
     */
    template <typename Iterator>
    Terse(Iterator const data, size_t const size, unsigned int const block=12) :
    d_signed(std::is_signed_v<typename std::iterator_traits<Iterator>::value_type>),
    d_block(block),
    d_size(size) {
        d_terse_frames.push_back(0);
        f_compress(data);
    }
    
    /**
     * @brief Reads in a Terse object that has been written to a file .
     *
     * Scans the stream for the Terse XML header, then reads the binary Terse data, leaving the stream position exactly on byte beyond the binary Terse data.
     *
     * @param istream The input stream containing Terse data.
     */
    Terse(std::ifstream& istream) : Terse(istream, XML_element(istream, "Terse")) {};
 
    /**
     * @brief Adds another frame to the Terse object. The new frame is defined by its begin iterator and size.
     *
     * The size must be the same as that of the first frame that was used for creating the Terse object.
     *
     * @tparam Iterator The type of the iterator.
     * @param data The starting iterator or pointer to the data.
     * @param size The number of elements to be encoded.
     */
    template <typename Iterator>
    void push_back(Iterator const data, size_t const size) {
        if (number_of_frames() == 0) {
            d_size = size;
            d_signed = std::is_signed_v<typename std::iterator_traits<Iterator>::value_type>;
        }
        else {
            assert(this->size() == size); // each frame of a multi-Terse object must have the same size
            assert(d_signed == std::is_signed_v<typename std::iterator_traits<Iterator>::value_type>);
        }
        d_terse_frames.push_back(0);
        f_compress(data);
    }

    /**
     * @brief Adds another frame to the Terse object. The new frame is defined by a reference to a container.
     *
     * The size must be the same as that of the first frame that was used for creating the Terse object.
     *
     * @tparam Container The type of the container containing integral data.
     * @param data The container containing integral data.
     */
    template <typename Container> requires requires (Container& c) {c.begin(), c.end(), c.size();}
    void push_back(Container const& data) {
        if constexpr(requires (Container &c) {c.dim();}) {
            for (int i = 0; i != data.dim().size(); ++i)
                if (number_of_frames() == 0)
                    d_dim.push_back(data.dim()[i]);
                else
                    assert(d_dim[i] == data.dim()[i]); // each frame of a multi-Terse object must have the same size
        }
        push_back(data.begin(), data.size());
    }

    /**
     * @brief Unpacks the Terse data and stores it in the provided container.
     *
     * Also asserts that the container is large enough.
     *
     * @tparam Container The type of the container.
     * @param data The container where the data will be stored.
     * @param frame The index of the frame to unpack (default is 0).
     */
    template <typename Container> requires requires (Container& c) {c.begin(), c.end(), c.size();}
    void prolix(Container& data, std::size_t frame = 0) {
        assert(this->size() == data.size());
        if constexpr(requires (Container &c) {c.dim();})
            for (int i = 0; i != d_dim.size(); ++i)
                assert(d_dim[i] == data.dim()[i]);
        assert(frame < number_of_frames());
        prolix(data.begin(), frame);
    }
    
    /**
     * @brief Unpacks the Terse data, storing it from the location defined by 'begin'.
     *
     * Unpacks with bounds checking.
     *
     * @tparam Iterator The type of the iterator.
     * @param begin The starting iterator or pointer where the data will be stored.
     * @param frame The index of the frame to unpack (default is 0).
     */
    template <typename Iterator> requires requires (Iterator& i) {*i;}
    void prolix(Iterator begin, std::size_t frame = 0) {
        assert(frame < number_of_frames());
        std::uint8_t const* terse_begin = f_find_terse_frame(frame);
        if (d_signed)
            assert(std::is_signed_v<typename std::iterator_traits<Iterator>::value_type>);
        Bit_pointer bitp(terse_begin);
        uint8_t significant_bits = 0;
        for (size_t from = 0; from < size(); from += d_block) {
            if (*bitp++ == 0) {
                significant_bits = Bit_range<const std::uint8_t*>(bitp,3);
                bitp += 3;
                if (significant_bits == 7) {
                    significant_bits += uint8_t(Bit_range<const std::uint8_t*>(bitp, 2));
                    bitp += 2;
                    if (significant_bits == 10) {
                        significant_bits += uint8_t(Bit_range<const std::uint8_t*>(bitp, 6));
                        bitp += 6;
                    }
                }
            }
            if (significant_bits == 0)
                std::fill(begin + from, begin + std::min(size(), from + d_block), 0);
            else {
                Bit_range<const std::uint8_t*> bitr(bitp, significant_bits);
                if constexpr (std::is_integral<typename std::iterator_traits<Iterator>::value_type>::value)
                    bitr.get_range(begin + from, begin + std::min(size(), from + d_block));
                else if (!is_signed())
                    for (auto i = from, to = std::min(size(), from + d_block); i < to; ++i, bitr.next())
                        begin[i] = double(std::uint64_t(bitr));
                else for (auto i = from, to = std::min(size(), from + d_block); i < to; ++i, bitr.next())
                    begin[i] = double(std::int64_t(bitr));
                bitp = bitr.begin();
            }
        }
        if (d_terse_frames.size() > ++frame)
            d_terse_frames[frame] = 1 + (bitp - Bit_pointer<const std::uint8_t*>(terse_begin)) / 8;
    }
    
    /**
     * @brief Returns the number of encoded elements of a single frame (all frames in a Terse object must have the same size).
     *
     * @return The number of encoded elements.
     */
    std::size_t const size() const {return d_size;}
    
    /**
     * @brief Returns the number of frames stored in the Terse object.
     *
     * @return The number of frames stored in the Terse object.
     */
    std::size_t const number_of_frames() const {return d_terse_frames.size();}
    
    /**
     * @brief Returns the dimensions of each of the Terse frames (all frames in a Terse object must have the same dimensions).
     *
     * @return The dimensions of each of the Terse frames.
     */
    std::vector<std::size_t> const& dim() const {return d_dim;}
    
    /**
     * @brief Sets the dimensions of the Terse frames. Since all frames must have the same dimensions, they can be set only once.
     *
     * @param dim The dimensions to set for the Terse frames.
     * @return A reference to the updated Terse object.
     */
    std::vector<std::size_t> const& dim(std::vector<std::size_t> const& dim) {
        assert(d_dim.size() == 0); // you cannot overwrite the dimensionality of a frame
        return d_dim = dim;
    }
    
    /**
     * @brief Returns true if the encoded data are signed, false if unsigned. Signed data cannot be decompressed into unsigned data.
     *
     * @return True if the encoded data are signed, false otherwise.
     */
    bool const is_signed() const {return d_signed;}
    
    /**
     * @brief Returns the number of bits per required for unpacking without overflows.
     *
     * @return The maximum number of bits per element.
     */
    unsigned const bits_per_val() const {return d_prolix_bits;}
    
    /**
     * @brief Returns the number of bytes used for encoding the Terse data.
     *
     * Mainly useful for report compression rates.
     *
     * @return The number of bytes used for encoding the Terse data.
     */
    std::size_t const terse_size() const {return d_terse_data.size();}
    
    /**
     * @brief Write the Terse object to the specified output stream.
     *
     * This member function first writes a small XML header with data required to unpack the Terse object,  then writes
     * the Terse object's data to the provided output stream.
     *
     * @param ostream The output stream to which the Terse data will be written.
     */
    void write(std::ostream& ostream) const {
        // Write Terse object attributes to the output stream
        ostream << "<Terse prolix_bits=\"" << d_prolix_bits << "\"";
        ostream << " signed=\"" << d_signed << "\"";
        ostream << " block=\"" << d_block << "\"";
        ostream << " memory_size=\"" << d_terse_data.size() * sizeof(std::uint8_t) << "\"";
        ostream << " number_of_values=\"" << size() << "\"";
        
        // Write Terse object dimensions if available
        if (!d_dim.empty()) {
            ostream << " dimensions=\"";
            for (size_t i = 0; i + 1 != d_dim.size(); ++i)
                ostream << d_dim[i] << " ";
            ostream << d_dim.back() << "\"";
        }
        ostream << " number_of_frames=\"" << d_terse_frames.size() << "\"/>";
        
        // Write Terse object data to the output stream
        ostream.write(reinterpret_cast<const char*>(d_terse_data.data()), d_terse_data.size());
        ostream.flush();
    }
    
private:
    bool d_signed;
    unsigned const d_block = 12;
    std::size_t d_size;
    unsigned d_prolix_bits = 0;
    std::vector<std::size_t> d_dim;
    std::vector<std::uint8_t> d_terse_data;
    std::vector<std::size_t> d_terse_frames;
    
    Terse(std::ifstream& istream, XML_element const& xmle) :
    d_prolix_bits(unsigned(std::stoul(xmle.attribute("prolix_bits")))),
    d_signed(std::stoul(xmle.attribute("signed"))),
    d_block(int(std::stoul(xmle.attribute("block")))),
    d_size(std::stoull(xmle.attribute("number_of_values"))) {
        std::string s = xmle.attribute("dimensions");
        std::istringstream iss(s);
        unsigned int val;
        while (iss >> val)
            d_dim.push_back(val);
        d_terse_data.resize(std::stold(xmle.attribute("memory_size")));
        istream.read((char*)&d_terse_data[0], d_terse_data.size());
        d_terse_frames.resize(std::stoull(xmle.attribute("number_of_frames")), 0);
    }
    
    template <typename Iterator>
    void const f_compress(Iterator data) {
        std::size_t prev_data_size = d_terse_data.size();
        d_terse_data.resize(prev_data_size + std::ceil(d_size * (sizeof(decltype(*data)) + (long double)(12.0) / (d_block * 8)) / sizeof(std::uint8_t)), 0);
        Bit_pointer bitp (d_terse_data.data() + prev_data_size);
        int prevbits = 0;
        for (size_t from = 0; from < d_size; from += d_block) {
            auto const to = std::min(d_size, from + d_block);
            typename std::iterator_traits<Iterator>::value_type setbits(0);
            auto p = data;
            for (auto i = from; i != to; ++i, ++p)
                if constexpr (std::is_unsigned_v<decltype(setbits)>)
                    setbits |= *p;
                else if constexpr (std::is_signed_v<decltype(setbits)>)
                    setbits |= std::abs(*p);
            unsigned significant_bits = f_highest_set_bit(setbits);
            d_prolix_bits = std::max(d_prolix_bits, significant_bits);
            if (prevbits == significant_bits) {
                (*bitp).set();
                ++bitp;
            }
            else {
                if (significant_bits < 7) {
                    Bit_range<std::uint8_t*>(++bitp, 3) |= significant_bits;
                    bitp += 3;
                }
                else if (significant_bits < 10) {
                    Bit_range<std::uint8_t*>(++bitp, 5) |= 0b111 + ((significant_bits - 7) << 3);
                    bitp += 5;
                }
                else {
                    Bit_range<std::uint8_t*>(++bitp, 11) |= 0b11111 + ((significant_bits - 10) << 5);
                    bitp += 11;
                }
                prevbits = significant_bits;
            }
            if (significant_bits != 0) {
                Bit_range<std::uint8_t*> r(bitp, significant_bits);
                r.append_range(data, data + (to - from));
                data += (to - from);
                bitp = r.begin();
            }
            else if constexpr (std::is_same_v<std::random_access_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>)
                data += d_block;
            else
                for (int i = 0; i != d_block; ++i, ++data);
        }
        d_terse_data.resize(1 + (bitp - d_terse_data.data()) / (sizeof(std::uint8_t) * 8));
        d_terse_data.shrink_to_fit();
    }
    
    template <typename T0>
    constexpr inline int const f_highest_set_bit(T0 val) const noexcept {
        if constexpr (std::is_signed_v<T0>)
            return (val == 0) ? 0 : 1 + f_highest_set_bit(std::make_unsigned_t<T0> (abs(val)));
        else {
            int r=0;
            for ( ; val; val>>=1, ++r);
            return r;
        }
    }
    
    std::uint8_t const* f_find_terse_frame(std::size_t frame) {
        if (frame > 0 && d_terse_frames[frame] == 0) {
            std::uint8_t const* terse_begin = f_find_terse_frame(frame - 1);
            Bit_pointer<const std::uint8_t*> bitp(terse_begin);
            uint8_t significant_bits = 0;
            for (size_t from = 0; from < size(); from += d_block) {
                if (*bitp++ == 0) {
                    significant_bits = Bit_range<const std::uint8_t*>(bitp,3);
                    bitp += 3;
                    if (significant_bits == 7) {
                        significant_bits += uint8_t(Bit_range<const std::uint8_t*>(bitp, 2));
                        bitp += 2;
                        if (significant_bits == 10) {
                            significant_bits += uint8_t(Bit_range<const std::uint8_t*>(bitp, 6));
                            bitp += 6;
                        }
                    }
                }
                bitp += significant_bits * d_block;
            }
            d_terse_frames[frame] = 1 + (bitp - Bit_pointer<const std::uint8_t*>(terse_begin)) / 8;
        }
        return d_terse_data.data() + d_terse_frames[frame];
    }
};

} // end namespace jpa

#endif /* Terse_h */
