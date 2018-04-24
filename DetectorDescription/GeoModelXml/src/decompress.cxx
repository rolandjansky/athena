/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Decompress a gzipped string to its original form 
//
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>

extern "C" {
    #include <zlib.h>
}

using namespace std;

string decompress(const string& str) {

    z_stream zs;                        // z_stream is zlib's control structure
//    std::fill_n(&zs, 1, 0);
//    memset(&zs, 0, sizeof(zs));
    zs.zalloc   = NULL;
    zs.zfree    = NULL;
    zs.opaque   = NULL;
    zs.next_in  = (Bytef *)str.c_str();
    zs.avail_in = str.size();

    // According to the official manual, 2nd param is windowsBits and is max. 15. But a web page somewhere 
    // said for gzip files, it should be 16 or'd (or added) to the usual windowsBits. By trial and error, the following works:
    int returnCode = inflateInit2(&zs, 0x1f);
    if (returnCode != Z_OK)
        throw runtime_error(string("decompress: inflateInit2 failed, error code means: ") + string(zError(returnCode)));

    char outBuffer[32768];
    string output;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outBuffer);
        zs.avail_out = sizeof(outBuffer);

        returnCode = inflate(&zs, 0);

        if (output.size() < zs.total_out) {
            output.append(outBuffer, zs.total_out - output.size());
        }

    } while (returnCode == Z_OK);

    inflateEnd(&zs);

    if (returnCode != Z_STREAM_END) { // an error occurred that was not EOF
        ostringstream oss;
        oss << "decompress: Exception during zlib decompression (code: " << returnCode << "): "
            << zs.msg;
        throw runtime_error(oss.str());
    }

    return output;
}
