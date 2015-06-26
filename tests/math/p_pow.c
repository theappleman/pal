#include <math.h>
#include "simple.h"

/*
 This implementation is based on work of Harrison Ainsworth.
 http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html
 */

/*
Copyright (c) 2007, Harrison Ainsworth / HXA7241.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define M_2POW23 8388608.0f

const unsigned int pow_mantissa_table_shift = 11;
unsigned int const pow_mantissa_table[] = {
0x58c, 0x10a4, 0x1bbd, 0x26d7, 0x31f2, 0x3d0e, 0x482b, 0x5349, 0x5e67,
0x6987, 0x74a8, 0x7fca, 0x8aec, 0x9610, 0xa135, 0xac5a, 0xb781,
0xc2a8, 0xcdd1, 0xd8fa, 0xe425, 0xef50, 0xfa7c, 0x105aa, 0x110d8,
0x11c07, 0x12737, 0x13269, 0x13d9b, 0x148ce, 0x15402, 0x15f37, 0x16a6d,
0x175a4, 0x180dc, 0x18c15, 0x1974f, 0x1a28a, 0x1adc6, 0x1b903, 0x1c440,
0x1cf7f, 0x1dabf, 0x1e600, 0x1f142, 0x1fc84, 0x207c8, 0x2130d, 0x21e52,
0x22999, 0x234e0, 0x24029, 0x24b72, 0x256bd, 0x26209, 0x26d55, 0x278a2,
0x283f1, 0x28f40, 0x29a91, 0x2a5e2, 0x2b134, 0x2bc88, 0x2c7dc, 0x2d331,
0x2de88, 0x2e9df, 0x2f537, 0x30090, 0x30beb, 0x31746, 0x322a2, 0x32dff,
0x3395d, 0x344bc, 0x3501d, 0x35b7e, 0x366e0, 0x37243, 0x37da7, 0x3890c,
0x39472, 0x39fd9, 0x3ab41, 0x3b6aa, 0x3c214, 0x3cd7f, 0x3d8eb, 0x3e458,
0x3efc6, 0x3fb35, 0x406a5, 0x41216, 0x41d88, 0x428fb, 0x4346e, 0x43fe3,
0x44b59, 0x456d0, 0x46248, 0x46dc1, 0x4793b, 0x484b6, 0x49031, 0x49bae,
0x4a72c, 0x4b2ab, 0x4be2b, 0x4c9ac, 0x4d52d, 0x4e0b0, 0x4ec34, 0x4f7b9,
0x5033f, 0x50ec6, 0x51a4d, 0x525d6, 0x53160, 0x53ceb, 0x54877, 0x55403,
0x55f91, 0x56b20, 0x576b0, 0x58241, 0x58dd2, 0x59965, 0x5a4f9, 0x5b08e,
0x5bc24, 0x5c7bb, 0x5d352, 0x5deeb, 0x5ea85, 0x5f620, 0x601bc, 0x60d59,
0x618f7, 0x62496, 0x63035, 0x63bd6, 0x64778, 0x6531b, 0x65ebf, 0x66a64,
0x6760a, 0x681b1, 0x68d59, 0x69902, 0x6a4ac, 0x6b057, 0x6bc03, 0x6c7b0,
0x6d35e, 0x6df0d, 0x6eabd, 0x6f66e, 0x70220, 0x70dd3, 0x71987, 0x7253c,
0x730f2, 0x73caa, 0x74862, 0x7541b, 0x75fd5, 0x76b90, 0x7774c, 0x7830a,
0x78ec8, 0x79a87, 0x7a648, 0x7b209, 0x7bdcb, 0x7c98f, 0x7d553, 0x7e118,
0x7ecdf, 0x7f8a6, 0x8046f, 0x81038, 0x81c03, 0x827ce, 0x8339b, 0x83f68,
0x84b37, 0x85706, 0x862d7, 0x86ea9, 0x87a7b, 0x8864f, 0x89224, 0x89dfa,
0x8a9d0, 0x8b5a8, 0x8c181, 0x8cd5b, 0x8d936, 0x8e512, 0x8f0ee, 0x8fccc,
0x908ab, 0x9148c, 0x9206d, 0x92c4f, 0x93832, 0x94416, 0x94ffb, 0x95be1,
0x967c8, 0x973b1, 0x97f9a, 0x98b84, 0x99770, 0x9a35c, 0x9af4a, 0x9bb38,
0x9c728, 0x9d318, 0x9df0a, 0x9eafc, 0x9f6f0, 0xa02e4, 0xa0eda, 0xa1ad1,
0xa26c9, 0xa32c2, 0xa3ebb, 0xa4ab6, 0xa56b2, 0xa62af, 0xa6ead, 0xa7aac,
0xa86ac, 0xa92ae, 0xa9eb0, 0xaaab3, 0xab6b7, 0xac2bc, 0xacec3, 0xadaca,
0xae6d2, 0xaf2dc, 0xafee6, 0xb0af2, 0xb16ff, 0xb230c, 0xb2f1b, 0xb3b2b,
0xb473b, 0xb534d, 0xb5f60, 0xb6b74, 0xb7789, 0xb839f, 0xb8fb6, 0xb9bce,
0xba7e7, 0xbb401, 0xbc01d, 0xbcc39, 0xbd856, 0xbe475, 0xbf094, 0xbfcb5,
0xc08d6, 0xc14f9, 0xc211c, 0xc2d41, 0xc3967, 0xc458e, 0xc51b5, 0xc5dde,
0xc6a08, 0xc7633, 0xc825f, 0xc8e8c, 0xc9abb, 0xca6ea, 0xcb31a, 0xcbf4c,
0xccb7e, 0xcd7b1, 0xce3e6, 0xcf01c, 0xcfc52, 0xd088a, 0xd14c3, 0xd20fc,
0xd2d37, 0xd3973, 0xd45b0, 0xd51ee, 0xd5e2d, 0xd6a6e, 0xd76af, 0xd82f1,
0xd8f34, 0xd9b79, 0xda7be, 0xdb405, 0xdc04c, 0xdcc95, 0xdd8df, 0xde52a,
0xdf176, 0xdfdc2, 0xe0a10, 0xe1660, 0xe22b0, 0xe2f01, 0xe3b53, 0xe47a6,
0xe53fb, 0xe6050, 0xe6ca7, 0xe78fe, 0xe8557, 0xe91b1, 0xe9e0c, 0xeaa68,
0xeb6c5, 0xec323, 0xecf82, 0xedbe2, 0xee843, 0xef4a5, 0xf0109, 0xf0d6d,
0xf19d3, 0xf263a, 0xf32a1, 0xf3f0a, 0xf4b74, 0xf57df, 0xf644b, 0xf70b8,
0xf7d26, 0xf8995, 0xf9605, 0xfa277, 0xfaee9, 0xfbb5d, 0xfc7d1, 0xfd447,
0xfe0be, 0xfed36, 0xff9af, 0x100629, 0x1012a4, 0x101f20, 0x102b9d, 0x10381c,
0x10449b, 0x10511c, 0x105d9d, 0x106a20, 0x1076a4, 0x108328, 0x108fae, 0x109c35,
0x10a8bd, 0x10b547, 0x10c1d1, 0x10ce5c, 0x10dae9, 0x10e776, 0x10f405, 0x110094,
0x110d25, 0x1119b7, 0x11264a, 0x1132de, 0x113f74, 0x114c0a, 0x1158a1, 0x116539,
0x1171d3, 0x117e6e, 0x118b09, 0x1197a6, 0x11a444, 0x11b0e3, 0x11bd83, 0x11ca24,
0x11d6c6, 0x11e36a, 0x11f00e, 0x11fcb4, 0x12095a, 0x121602, 0x1222ab, 0x122f55,
0x123c00, 0x1248ac, 0x125559, 0x126208, 0x126eb7, 0x127b68, 0x128819, 0x1294cc,
0x12a180, 0x12ae35, 0x12baeb, 0x12c7a2, 0x12d45a, 0x12e113, 0x12edce, 0x12fa89,
0x130746, 0x131404, 0x1320c3, 0x132d83, 0x133a44, 0x134706, 0x1353c9, 0x13608e,
0x136d53, 0x137a1a, 0x1386e1, 0x1393aa, 0x13a074, 0x13ad3f, 0x13ba0b, 0x13c6d8,
0x13d3a7, 0x13e076, 0x13ed47, 0x13fa18, 0x1406eb, 0x1413bf, 0x142094, 0x142d6a,
0x143a42, 0x14471a, 0x1453f3, 0x1460ce, 0x146daa, 0x147a86, 0x148764, 0x149443,
0x14a124, 0x14ae05, 0x14bae7, 0x14c7ca, 0x14d4af, 0x14e195, 0x14ee7c, 0x14fb64,
0x15084d, 0x151537, 0x152222, 0x152f0f, 0x153bfc, 0x1548eb, 0x1555db, 0x1562cc,
0x156fbe, 0x157cb1, 0x1589a5, 0x15969b, 0x15a391, 0x15b089, 0x15bd82, 0x15ca7c,
0x15d777, 0x15e473, 0x15f170, 0x15fe6e, 0x160b6e, 0x16186f, 0x162570, 0x163273,
0x163f78, 0x164c7c, 0x165983, 0x16668a, 0x167393, 0x16809c, 0x168da8, 0x169ab3,
0x16a7c0, 0x16b4ce, 0x16c1de, 0x16ceee, 0x16dc00, 0x16e913, 0x16f627, 0x17033c,
0x171052, 0x171d69, 0x172a82, 0x17379b, 0x1744b6, 0x1751d1, 0x175eee, 0x176c0c,
0x17792c, 0x17864c, 0x17936e, 0x17a090, 0x17adb4, 0x17bad9, 0x17c7ff, 0x17d526,
0x17e24e, 0x17ef78, 0x17fca2, 0x1809ce, 0x1816fb, 0x182429, 0x183158, 0x183e88,
0x184bba, 0x1858ed, 0x186620, 0x187355, 0x18808b, 0x188dc2, 0x189afb, 0x18a834,
0x18b56f, 0x18c2ab, 0x18cfe8, 0x18dd26, 0x18ea65, 0x18f7a5, 0x1904e7, 0x191229,
0x191f6d, 0x192cb2, 0x1939f8, 0x19473f, 0x195488, 0x1961d1, 0x196f1c, 0x197c68,
0x1989b5, 0x199703, 0x19a452, 0x19b1a3, 0x19bef4, 0x19cc47, 0x19d99b, 0x19e6f0,
0x19f447, 0x1a019e, 0x1a0ef6, 0x1a1c50, 0x1a29ab, 0x1a3707, 0x1a4464, 0x1a51c2,
0x1a5f22, 0x1a6c83, 0x1a79e4, 0x1a8747, 0x1a94ac, 0x1aa211, 0x1aaf77, 0x1abcdf,
0x1aca48, 0x1ad7b2, 0x1ae51d, 0x1af289, 0x1afff6, 0x1b0d65, 0x1b1ad5, 0x1b2846,
0x1b35b8, 0x1b432b, 0x1b50a0, 0x1b5e15, 0x1b6b8c, 0x1b7904, 0x1b867d, 0x1b93f7,
0x1ba172, 0x1baeef, 0x1bbc6d, 0x1bc9ec, 0x1bd76c, 0x1be4ed, 0x1bf270, 0x1bfff3,
0x1c0d78, 0x1c1afe, 0x1c2885, 0x1c360d, 0x1c4397, 0x1c5121, 0x1c5ead, 0x1c6c3a,
0x1c79c8, 0x1c8758, 0x1c94e8, 0x1ca27a, 0x1cb00d, 0x1cbda0, 0x1ccb36, 0x1cd8cc,
0x1ce664, 0x1cf3fc, 0x1d0196, 0x1d0f32, 0x1d1cce, 0x1d2a6b, 0x1d380a, 0x1d45aa,
0x1d534b, 0x1d60ed, 0x1d6e90, 0x1d7c35, 0x1d89db, 0x1d9781, 0x1da52a, 0x1db2d3,
0x1dc07d, 0x1dce29, 0x1ddbd6, 0x1de984, 0x1df733, 0x1e04e3, 0x1e1295, 0x1e2048,
0x1e2dfc, 0x1e3bb1, 0x1e4967, 0x1e571e, 0x1e64d7, 0x1e7291, 0x1e804c, 0x1e8e08,
0x1e9bc6, 0x1ea984, 0x1eb744, 0x1ec505, 0x1ed2c8, 0x1ee08b, 0x1eee50, 0x1efc15,
0x1f09dc, 0x1f17a4, 0x1f256e, 0x1f3338, 0x1f4104, 0x1f4ed1, 0x1f5ca0, 0x1f6a6f,
0x1f783f, 0x1f8611, 0x1f93e4, 0x1fa1b8, 0x1faf8e, 0x1fbd64, 0x1fcb3c, 0x1fd915,
0x1fe6ef, 0x1ff4ca, 0x2002a7, 0x201084, 0x201e64, 0x202c44, 0x203a25, 0x204808,
0x2055ec, 0x2063d0, 0x2071b6, 0x207f9e, 0x208d86, 0x209b70, 0x20a95b, 0x20b747,
0x20c535, 0x20d323, 0x20e113, 0x20ef04, 0x20fcf6, 0x210aea, 0x2118de, 0x2126d4,
0x2134cc, 0x2142c4, 0x2150bd, 0x215eb8, 0x216cb4, 0x217ab1, 0x2188af, 0x2196ae,
0x21a4b0, 0x21b2b1, 0x21c0b5, 0x21ceb9, 0x21dcbf, 0x21eac6, 0x21f8ce, 0x2206d7,
0x2214e1, 0x2222ed, 0x2230fa, 0x223f08, 0x224d18, 0x225b28, 0x22693a, 0x22774c,
0x228561, 0x229376, 0x22a18d, 0x22afa5, 0x22bdbe, 0x22cbd8, 0x22d9f4, 0x22e810,
0x22f62f, 0x23044e, 0x23126e, 0x232090, 0x232eb3, 0x233cd7, 0x234afc, 0x235923,
0x23674b, 0x237574, 0x23839e, 0x2391c9, 0x239ff6, 0x23ae24, 0x23bc53, 0x23ca84,
0x23d8b5, 0x23e6e8, 0x23f51c, 0x240351, 0x241188, 0x241fc0, 0x242df9, 0x243c33,
0x244a6e, 0x2458ab, 0x2466e9, 0x247528, 0x248369, 0x2491aa, 0x249fed, 0x24ae31,
0x24bc77, 0x24cabd, 0x24d905, 0x24e74e, 0x24f598, 0x2503e4, 0x251231, 0x25207e,
0x252ece, 0x253d1e, 0x254b70, 0x2559c3, 0x256818, 0x25766d, 0x2584c4, 0x25931b,
0x25a175, 0x25afcf, 0x25be2b, 0x25cc88, 0x25dae6, 0x25e945, 0x25f7a6, 0x260608,
0x26146b, 0x2622cf, 0x263135, 0x263f9c, 0x264e04, 0x265c6d, 0x266ad8, 0x267944,
0x2687b1, 0x26961f, 0x26a48f, 0x26b300, 0x26c172, 0x26cfe5, 0x26de5a, 0x26ecd0,
0x26fb47, 0x2709bf, 0x271839, 0x2726b4, 0x273530, 0x2743ae, 0x27522c, 0x2760ac,
0x276f2e, 0x277db0, 0x278c34, 0x279ab8, 0x27a93f, 0x27b7c6, 0x27c64f, 0x27d4d9,
0x27e364, 0x27f1f0, 0x28007e, 0x280f0e, 0x281d9e, 0x282c2f, 0x283ac2, 0x284956,
0x2857ec, 0x286682, 0x28751a, 0x2883b3, 0x28924e, 0x28a0e9, 0x28af86, 0x28be24,
0x28ccc4, 0x28db64, 0x28ea07, 0x28f8aa, 0x29074e, 0x2915f4, 0x29249b, 0x293344,
0x2941ed, 0x295098, 0x295f44, 0x296df2, 0x297ca0, 0x298b50, 0x299a02, 0x29a8b4,
0x29b768, 0x29c61d, 0x29d4d4, 0x29e38b, 0x29f244, 0x2a00fe, 0x2a0fba, 0x2a1e76,
0x2a2d34, 0x2a3bf3, 0x2a4ab4, 0x2a5976, 0x2a6839, 0x2a76fd, 0x2a85c3, 0x2a948a,
0x2aa352, 0x2ab21c, 0x2ac0e6, 0x2acfb2, 0x2ade80, 0x2aed4e, 0x2afc1e, 0x2b0af0,
0x2b19c2, 0x2b2896, 0x2b376b, 0x2b4641, 0x2b5519, 0x2b63f2, 0x2b72cc, 0x2b81a8,
0x2b9084, 0x2b9f62, 0x2bae42, 0x2bbd22, 0x2bcc04, 0x2bdae8, 0x2be9cc, 0x2bf8b2,
0x2c0799, 0x2c1681, 0x2c256b, 0x2c3456, 0x2c4342, 0x2c5230, 0x2c611f, 0x2c700f,
0x2c7f00, 0x2c8df3, 0x2c9ce7, 0x2cabdc, 0x2cbad3, 0x2cc9cb, 0x2cd8c4, 0x2ce7be,
0x2cf6ba, 0x2d05b8, 0x2d14b6, 0x2d23b6, 0x2d32b7, 0x2d41b9, 0x2d50bd, 0x2d5fc2,
0x2d6ec8, 0x2d7dcf, 0x2d8cd8, 0x2d9be2, 0x2daaee, 0x2db9fa, 0x2dc908, 0x2dd818,
0x2de728, 0x2df63a, 0x2e054e, 0x2e1462, 0x2e2378, 0x2e328f, 0x2e41a8, 0x2e50c1,
0x2e5fdc, 0x2e6ef9, 0x2e7e17, 0x2e8d36, 0x2e9c56, 0x2eab78, 0x2eba9a, 0x2ec9bf,
0x2ed8e4, 0x2ee80b, 0x2ef734, 0x2f065d, 0x2f1588, 0x2f24b4, 0x2f33e2, 0x2f4310,
0x2f5240, 0x2f6172, 0x2f70a4, 0x2f7fd8, 0x2f8f0e, 0x2f9e44, 0x2fad7c, 0x2fbcb6,
0x2fcbf0, 0x2fdb2c, 0x2fea6a, 0x2ff9a8, 0x3008e8, 0x30182a, 0x30276c, 0x3036b0,
0x3045f5, 0x30553c, 0x306484, 0x3073cc, 0x308317, 0x309263, 0x30a1b0, 0x30b0fe,
0x30c04e, 0x30cfa0, 0x30def2, 0x30ee46, 0x30fd9b, 0x310cf1, 0x311c49, 0x312ba2,
0x313afd, 0x314a58, 0x3159b6, 0x316914, 0x317874, 0x3187d5, 0x319737, 0x31a69b,
0x31b600, 0x31c566, 0x31d4ce, 0x31e437, 0x31f3a2, 0x32030e, 0x32127b, 0x3221e9,
0x323159, 0x3240ca, 0x32503c, 0x325fb0, 0x326f26, 0x327e9c, 0x328e14, 0x329d8c,
0x32ad07, 0x32bc83, 0x32cc00, 0x32db7e, 0x32eafe, 0x32fa80, 0x330a02, 0x331986,
0x33290c, 0x333892, 0x33481a, 0x3357a3, 0x33672e, 0x3376ba, 0x338647, 0x3395d6,
0x33a566, 0x33b4f7, 0x33c48a, 0x33d41e, 0x33e3b4, 0x33f34a, 0x3402e2, 0x34127c,
0x342216, 0x3431b2, 0x344150, 0x3450ef, 0x34608f, 0x347030, 0x347fd4, 0x348f78,
0x349f1e, 0x34aec4, 0x34be6d, 0x34ce16, 0x34ddc2, 0x34ed6e, 0x34fd1c, 0x350ccb,
0x351c7c, 0x352c2d, 0x353be0, 0x354b95, 0x355b4b, 0x356b02, 0x357abb, 0x358a75,
0x359a30, 0x35a9ed, 0x35b9ac, 0x35c96a, 0x35d92c, 0x35e8ee, 0x35f8b2, 0x360876,
0x36183c, 0x362804, 0x3637ce, 0x364798, 0x365764, 0x366730, 0x367700, 0x3686cf,
0x3696a0, 0x36a673, 0x36b647, 0x36c61c, 0x36d5f3, 0x36e5cb, 0x36f5a4, 0x370580,
0x37155c, 0x372539, 0x373518, 0x3744f8, 0x3754da, 0x3764be, 0x3774a2, 0x378488,
0x37946f, 0x37a458, 0x37b442, 0x37c42d, 0x37d41a, 0x37e408, 0x37f3f8, 0x3803e8,
0x3813db, 0x3823ce, 0x3833c4, 0x3843ba, 0x3853b2, 0x3863ab, 0x3873a6, 0x3883a2,
0x38939f, 0x38a39e, 0x38b39e, 0x38c39f, 0x38d3a2, 0x38e3a6, 0x38f3ac, 0x3903b3,
0x3913bc, 0x3923c5, 0x3933d0, 0x3943dd, 0x3953eb, 0x3963fa, 0x39740c, 0x39841e,
0x399431, 0x39a446, 0x39b45c, 0x39c474, 0x39d48e, 0x39e4a8, 0x39f4c4, 0x3a04e1,
0x3a1500, 0x3a2520, 0x3a3542, 0x3a4564, 0x3a5589, 0x3a65ae, 0x3a75d6, 0x3a85fe,
0x3a9628, 0x3aa654, 0x3ab680, 0x3ac6ae, 0x3ad6de, 0x3ae70e, 0x3af741, 0x3b0774,
0x3b17aa, 0x3b27e0, 0x3b3818, 0x3b4852, 0x3b588c, 0x3b68c8, 0x3b7906, 0x3b8945,
0x3b9986, 0x3ba9c8, 0x3bba0b, 0x3bca4f, 0x3bda96, 0x3beadc, 0x3bfb26, 0x3c0b70,
0x3c1bbc, 0x3c2c08, 0x3c3c57, 0x3c4ca7, 0x3c5cf8, 0x3c6d4b, 0x3c7d9f, 0x3c8df4,
0x3c9e4c, 0x3caea4, 0x3cbefe, 0x3ccf59, 0x3cdfb6, 0x3cf014, 0x3d0074, 0x3d10d4,
0x3d2136, 0x3d319a, 0x3d4200, 0x3d5266, 0x3d62ce, 0x3d7338, 0x3d83a2, 0x3d940e,
0x3da47c, 0x3db4ec, 0x3dc55c, 0x3dd5ce, 0x3de642, 0x3df6b6, 0x3e072c, 0x3e17a4,
0x3e281d, 0x3e3898, 0x3e4914, 0x3e5991, 0x3e6a10, 0x3e7a90, 0x3e8b12, 0x3e9b95,
0x3eac1a, 0x3ebc9f, 0x3ecd27, 0x3eddb0, 0x3eee3a, 0x3efec6, 0x3f0f52, 0x3f1fe1,
0x3f3072, 0x3f4102, 0x3f5196, 0x3f622a, 0x3f72c0, 0x3f8356, 0x3f93f0, 0x3fa48a,
0x3fb525, 0x3fc5c2, 0x3fd660, 0x3fe700, 0x3ff7a2, 0x400844, 0x4018e8, 0x40298e,
0x403a35, 0x404add, 0x405b87, 0x406c32, 0x407cdf, 0x408d8d, 0x409e3d, 0x40aeee,
0x40bfa1, 0x40d055, 0x40e10a, 0x40f1c1, 0x41027a, 0x411333, 0x4123ef, 0x4134ab,
0x41456a, 0x415629, 0x4166ea, 0x4177ac, 0x418871, 0x419936, 0x41a9fd, 0x41bac5,
0x41cb8f, 0x41dc5a, 0x41ed27, 0x41fdf5, 0x420ec5, 0x421f96, 0x423068, 0x42413c,
0x425212, 0x4262e8, 0x4273c1, 0x42849b, 0x429576, 0x42a653, 0x42b731, 0x42c811,
0x42d8f2, 0x42e9d4, 0x42fab8, 0x430b9e, 0x431c85, 0x432d6d, 0x433e58, 0x434f43,
0x436030, 0x43711e, 0x43820e, 0x4392ff, 0x43a3f2, 0x43b4e6, 0x43c5dc, 0x43d6d2,
0x43e7cb, 0x43f8c5, 0x4409c1, 0x441abe, 0x442bbc, 0x443cbc, 0x444dbe, 0x445ec0,
0x446fc5, 0x4480ca, 0x4491d2, 0x44a2db, 0x44b3e5, 0x44c4f1, 0x44d5fe, 0x44e70c,
0x44f81d, 0x45092e, 0x451a42, 0x452b56, 0x453c6c, 0x454d84, 0x455e9d, 0x456fb8,
0x4580d4, 0x4591f1, 0x45a310, 0x45b430, 0x45c552, 0x45d676, 0x45e79b, 0x45f8c1,
0x4609e9, 0x461b12, 0x462c3e, 0x463d6a, 0x464e98, 0x465fc7, 0x4670f8, 0x46822a,
0x46935e, 0x46a493, 0x46b5ca, 0x46c702, 0x46d83c, 0x46e977, 0x46fab4, 0x470bf2,
0x471d32, 0x472e73, 0x473fb6, 0x4750fa, 0x476240, 0x477387, 0x4784d0, 0x47961a,
0x47a766, 0x47b8b3, 0x47ca02, 0x47db52, 0x47eca4, 0x47fdf7, 0x480f4c, 0x4820a2,
0x4831fa, 0x484353, 0x4854ae, 0x48660a, 0x487768, 0x4888c6, 0x489a28, 0x48ab8a,
0x48bcee, 0x48ce52, 0x48dfba, 0x48f122, 0x49028c, 0x4913f7, 0x492564, 0x4936d2,
0x494842, 0x4959b3, 0x496b26, 0x497c9a, 0x498e10, 0x499f88, 0x49b100, 0x49c27b,
0x49d3f7, 0x49e574, 0x49f6f3, 0x4a0874, 0x4a19f6, 0x4a2b79, 0x4a3cfe, 0x4a4e85,
0x4a600d, 0x4a7196, 0x4a8322, 0x4a94ae, 0x4aa63c, 0x4ab7cc, 0x4ac95d, 0x4adaf0,
0x4aec84, 0x4afe1a, 0x4b0fb1, 0x4b214a, 0x4b32e4, 0x4b4480, 0x4b561e, 0x4b67bc,
0x4b795d, 0x4b8aff, 0x4b9ca2, 0x4bae47, 0x4bbfee, 0x4bd196, 0x4be340, 0x4bf4eb,
0x4c0698, 0x4c1846, 0x4c29f6, 0x4c3ba6, 0x4c4d5a, 0x4c5f0e, 0x4c70c4, 0x4c827b,
0x4c9434, 0x4ca5ee, 0x4cb7aa, 0x4cc968, 0x4cdb27, 0x4cece8, 0x4cfeaa, 0x4d106e,
0x4d2233, 0x4d33fa, 0x4d45c2, 0x4d578c, 0x4d6957, 0x4d7b24, 0x4d8cf3, 0x4d9ec2,
0x4db094, 0x4dc267, 0x4dd43c, 0x4de612, 0x4df7ea, 0x4e09c3, 0x4e1b9e, 0x4e2d7a,
0x4e3f58, 0x4e5138, 0x4e6319, 0x4e74fc, 0x4e86e0, 0x4e98c5, 0x4eaaad, 0x4ebc95,
0x4ece80, 0x4ee06c, 0x4ef259, 0x4f0448, 0x4f1639, 0x4f282b, 0x4f3a1e, 0x4f4c14,
0x4f5e0a, 0x4f7002, 0x4f81fd, 0x4f93f8, 0x4fa5f5, 0x4fb7f4, 0x4fc9f4, 0x4fdbf5,
0x4fedf9, 0x4ffffe, 0x501204, 0x50240c, 0x503616, 0x504820, 0x505a2d, 0x506c3b,
0x507e4b, 0x50905c, 0x50a26f, 0x50b484, 0x50c69a, 0x50d8b1, 0x50eaca, 0x50fce5,
0x510f01, 0x51211f, 0x51333e, 0x514560, 0x515782, 0x5169a6, 0x517bcc, 0x518df3,
0x51a01c, 0x51b246, 0x51c472, 0x51d6a0, 0x51e8cf, 0x51fb00, 0x520d32, 0x521f66,
0x52319c, 0x5243d2, 0x52560b, 0x526845, 0x527a81, 0x528cbe, 0x529efe, 0x52b13e,
0x52c380, 0x52d5c4, 0x52e809, 0x52fa50, 0x530c98, 0x531ee2, 0x53312e, 0x53437b,
0x5355ca, 0x53681a, 0x537a6c, 0x538cc0, 0x539f15, 0x53b16c, 0x53c3c4, 0x53d61e,
0x53e879, 0x53fad6, 0x540d35, 0x541f95, 0x5431f7, 0x54445a, 0x5456c0, 0x546926,
0x547b8e, 0x548df8, 0x54a064, 0x54b2d0, 0x54c53f, 0x54d7af, 0x54ea21, 0x54fc94,
0x550f0a, 0x552180, 0x5533f8, 0x554672, 0x5558ee, 0x556b6a, 0x557de9, 0x559069,
0x55a2eb, 0x55b56e, 0x55c7f4, 0x55da7a, 0x55ed02, 0x55ff8c, 0x561218, 0x5624a4,
0x563734, 0x5649c4, 0x565c56, 0x566ee9, 0x56817e, 0x569415, 0x56a6ad, 0x56b947,
0x56cbe2, 0x56de80, 0x56f11e, 0x5703bf, 0x571661, 0x572904, 0x573baa, 0x574e50,
0x5760f9, 0x5773a3, 0x57864f, 0x5798fc, 0x57abab, 0x57be5b, 0x57d10e, 0x57e3c1,
0x57f677, 0x58092e, 0x581be6, 0x582ea0, 0x58415c, 0x58541a, 0x5866d9, 0x58799a,
0x588c5c, 0x589f20, 0x58b1e6, 0x58c4ad, 0x58d776, 0x58ea40, 0x58fd0d, 0x590fda,
0x5922aa, 0x59357b, 0x59484e, 0x595b22, 0x596df8, 0x5980cf, 0x5993a9, 0x59a683,
0x59b960, 0x59cc3e, 0x59df1e, 0x59f1ff, 0x5a04e2, 0x5a17c6, 0x5a2aad, 0x5a3d95,
0x5a507e, 0x5a636a, 0x5a7656, 0x5a8945, 0x5a9c35, 0x5aaf27, 0x5ac21a, 0x5ad50f,
0x5ae806, 0x5afafe, 0x5b0df8, 0x5b20f4, 0x5b33f1, 0x5b46f0, 0x5b59f0, 0x5b6cf2,
0x5b7ff6, 0x5b92fc, 0x5ba603, 0x5bb90c, 0x5bcc16, 0x5bdf22, 0x5bf230, 0x5c0540,
0x5c1850, 0x5c2b63, 0x5c3e78, 0x5c518e, 0x5c64a5, 0x5c77be, 0x5c8ada, 0x5c9df6,
0x5cb114, 0x5cc434, 0x5cd756, 0x5cea79, 0x5cfd9e, 0x5d10c4, 0x5d23ed, 0x5d3716,
0x5d4a42, 0x5d5d6f, 0x5d709e, 0x5d83cf, 0x5d9701, 0x5daa35, 0x5dbd6a, 0x5dd0a2,
0x5de3da, 0x5df715, 0x5e0a51, 0x5e1d8f, 0x5e30ce, 0x5e4410, 0x5e5752, 0x5e6a97,
0x5e7ddd, 0x5e9125, 0x5ea46f, 0x5eb7ba, 0x5ecb07, 0x5ede55, 0x5ef1a6, 0x5f04f7,
0x5f184b, 0x5f2ba0, 0x5f3ef7, 0x5f5250, 0x5f65aa, 0x5f7906, 0x5f8c64, 0x5f9fc2,
0x5fb324, 0x5fc686, 0x5fd9eb, 0x5fed50, 0x6000b8, 0x601422, 0x60278d, 0x603afa,
0x604e68, 0x6061d8, 0x60754a, 0x6088bd, 0x609c33, 0x60afa9, 0x60c322, 0x60d69c,
0x60ea18, 0x60fd96, 0x611115, 0x612496, 0x613819, 0x614b9d, 0x615f23, 0x6172ab,
0x618634, 0x6199bf, 0x61ad4c, 0x61c0db, 0x61d46b, 0x61e7fd, 0x61fb90, 0x620f26,
0x6222bd, 0x623656, 0x6249f0, 0x625d8c, 0x62712a, 0x6284c9, 0x62986a, 0x62ac0e,
0x62bfb2, 0x62d358, 0x62e700, 0x62faaa, 0x630e56, 0x632203, 0x6335b1, 0x634962,
0x635d14, 0x6370c8, 0x63847e, 0x639835, 0x63abee, 0x63bfa9, 0x63d365, 0x63e724,
0x63fae4, 0x640ea5, 0x642268, 0x64362e, 0x6449f4, 0x645dbd, 0x647187, 0x648553,
0x649920, 0x64acf0, 0x64c0c1, 0x64d494, 0x64e868, 0x64fc3e, 0x651016, 0x6523f0,
0x6537cb, 0x654ba8, 0x655f87, 0x657368, 0x65874a, 0x659b2e, 0x65af13, 0x65c2fb,
0x65d6e4, 0x65eacf, 0x65febb, 0x6612aa, 0x66269a, 0x663a8c, 0x664e7f, 0x666274,
0x66766b, 0x668a64, 0x669e5e, 0x66b25a, 0x66c658, 0x66da58, 0x66ee59, 0x67025c,
0x671661, 0x672a68, 0x673e70, 0x67527a, 0x676686, 0x677a93, 0x678ea3, 0x67a2b4,
0x67b6c6, 0x67cadb, 0x67def1, 0x67f309, 0x680722, 0x681b3e, 0x682f5b, 0x68437a,
0x68579b, 0x686bbd, 0x687fe1, 0x689407, 0x68a82f, 0x68bc58, 0x68d083, 0x68e4b0,
0x68f8df, 0x690d0f, 0x692141, 0x693575, 0x6949aa, 0x695de2, 0x69721b, 0x698656,
0x699a92, 0x69aed1, 0x69c311, 0x69d753, 0x69eb96, 0x69ffdc, 0x6a1423, 0x6a286c,
0x6a3cb7, 0x6a5103, 0x6a6551, 0x6a79a1, 0x6a8df3, 0x6aa246, 0x6ab69c, 0x6acaf3,
0x6adf4b, 0x6af3a6, 0x6b0802, 0x6b1c60, 0x6b30c0, 0x6b4522, 0x6b5985, 0x6b6dea,
0x6b8251, 0x6b96ba, 0x6bab24, 0x6bbf90, 0x6bd3fe, 0x6be86e, 0x6bfcdf, 0x6c1153,
0x6c25c8, 0x6c3a3e, 0x6c4eb7, 0x6c6331, 0x6c77ae, 0x6c8c2b, 0x6ca0ab, 0x6cb52c,
0x6cc9b0, 0x6cde35, 0x6cf2bc, 0x6d0744, 0x6d1bce, 0x6d305b, 0x6d44e8, 0x6d5978,
0x6d6e0a, 0x6d829d, 0x6d9732, 0x6dabc9, 0x6dc061, 0x6dd4fc, 0x6de998, 0x6dfe36,
0x6e12d6, 0x6e2777, 0x6e3c1a, 0x6e50c0, 0x6e6566, 0x6e7a0f, 0x6e8eba, 0x6ea366,
0x6eb814, 0x6eccc4, 0x6ee176, 0x6ef629, 0x6f0ade, 0x6f1f95, 0x6f344e, 0x6f4909,
0x6f5dc5, 0x6f7283, 0x6f8743, 0x6f9c05, 0x6fb0c9, 0x6fc58e, 0x6fda55, 0x6fef1e,
0x7003e9, 0x7018b6, 0x702d84, 0x704254, 0x705726, 0x706bfa, 0x7080d0, 0x7095a7,
0x70aa80, 0x70bf5c, 0x70d438, 0x70e917, 0x70fdf8, 0x7112da, 0x7127be, 0x713ca4,
0x71518c, 0x716675, 0x717b60, 0x71904e, 0x71a53c, 0x71ba2d, 0x71cf20, 0x71e414,
0x71f90a, 0x720e03, 0x7222fc, 0x7237f8, 0x724cf6, 0x7261f5, 0x7276f6, 0x728bf9,
0x72a0fe, 0x72b604, 0x72cb0d, 0x72e017, 0x72f523, 0x730a31, 0x731f41, 0x733452,
0x734966, 0x735e7b, 0x737392, 0x7388aa, 0x739dc5, 0x73b2e2, 0x73c800, 0x73dd20,
0x73f242, 0x740766, 0x741c8c, 0x7431b3, 0x7446dc, 0x745c08, 0x747135, 0x748663,
0x749b94, 0x74b0c6, 0x74c5fb, 0x74db31, 0x74f069, 0x7505a3, 0x751adf, 0x75301c,
0x75455b, 0x755a9d, 0x756fe0, 0x758525, 0x759a6b, 0x75afb4, 0x75c4fe, 0x75da4b,
0x75ef99, 0x7604e9, 0x761a3a, 0x762f8e, 0x7644e4, 0x765a3b, 0x766f94, 0x7684ef,
0x769a4c, 0x76afab, 0x76c50b, 0x76da6e, 0x76efd2, 0x770538, 0x771aa0, 0x77300a,
0x774576, 0x775ae4, 0x777053, 0x7785c4, 0x779b37, 0x77b0ac, 0x77c623, 0x77db9c,
0x77f117, 0x780693, 0x781c11, 0x783192, 0x784714, 0x785c98, 0x78721d, 0x7887a5,
0x789d2e, 0x78b2ba, 0x78c847, 0x78ddd6, 0x78f367, 0x7908fa, 0x791e8f, 0x793425,
0x7949be, 0x795f58, 0x7974f4, 0x798a92, 0x79a032, 0x79b5d4, 0x79cb78, 0x79e11d,
0x79f6c5, 0x7a0c6e, 0x7a2219, 0x7a37c6, 0x7a4d75, 0x7a6326, 0x7a78d9, 0x7a8e8d,
0x7aa444, 0x7ab9fc, 0x7acfb6, 0x7ae573, 0x7afb31, 0x7b10f0, 0x7b26b2, 0x7b3c76,
0x7b523b, 0x7b6803, 0x7b7dcc, 0x7b9397, 0x7ba964, 0x7bbf33, 0x7bd504, 0x7bead7,
0x7c00ac, 0x7c1682, 0x7c2c5b, 0x7c4235, 0x7c5811, 0x7c6def, 0x7c83cf, 0x7c99b1,
0x7caf95, 0x7cc57b, 0x7cdb62, 0x7cf14c, 0x7d0737, 0x7d1d25, 0x7d3314, 0x7d4905,
0x7d5ef8, 0x7d74ed, 0x7d8ae4, 0x7da0dc, 0x7db6d7, 0x7dccd3, 0x7de2d2, 0x7df8d2,
0x7e0ed4, 0x7e24d8, 0x7e3adf, 0x7e50e6, 0x7e66f0, 0x7e7cfc, 0x7e930a, 0x7ea919,
0x7ebf2b, 0x7ed53e, 0x7eeb54, 0x7f016b, 0x7f1784, 0x7f2d9f, 0x7f43bc, 0x7f59db,
0x7f6ffc, 0x7f861f, 0x7f9c43, 0x7fb26a, 0x7fc892, 0x7fdebd, 0x7ff4e9
};
/*

 The pow mantissa table above has been generated with the following code :
#define POW_NEED_GENERATE_TABLE

*/
#ifdef POW_NEED_GENERATE_TABLE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_2POW23 8388608.0f     // 2^23
const uint32_t precision = 11;
/*

This generating the mantissa table of size (2 ^ precision)
precision is the number of mantissa bits, must be between >= 0 and <= 18

*/
int main(int argc, char *argv[])
{
    const float fprecision = 1 << precision;
    float zeroToOne = 1.0f / (fprecision * 2.0f);

    printf("const uint32_t pow_mantissa_table_shift = %u;\n", precision);
    puts("uint32_t const pow_mantissa_table[] = {");
    int i;
    for (i = 0; i < (1 << precision); i++) {
        const float f = ((float)powf(2.0f, zeroToOne) - 1.0f) * M_2POW23;
        mantissa = (uint32_t)(f < M_2POW23 ? f : (M_2POW23 - 1.0f));
        printf("0x%x, ", mantissa);

        zeroToOne += 1.0f / fprecision;
	if(i && ((i%8) == 0))
	    puts("");
    }
    puts("};");
}
#endif

/**
 * pow_lookup(exponent, ln(base))
 *
 * @exp        power to raise radix to (exponent)
 * @ilog2      one over log, to required radix, of two (ln(base))
 */
float pow_lookup(const float exp, const float log_base)
{
    union {
        float f;
        uint32_t x;
    } u;
    const uint32_t i = (uint32_t) ((exp * M_2POW23 * log_base * M_LOG2E) + (127.0f * M_2POW23));
    u.x = (i & 0xFF800000) | pow_mantissa_table[(i & 0x7FFFFF) >> (23 - pow_mantissa_table_shift)];
    return u.f;
}


void generate_ref(float *out, size_t n)
{
    size_t i;

    p_ln_f32(ai, out, n);
    for (i = 0; i < n; i++)
        out[i] = pow_lookup(bi[i], out[i]);
}
