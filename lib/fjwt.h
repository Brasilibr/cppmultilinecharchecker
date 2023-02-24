
#ifndef fjwt_h
#define fjwt_h

#include <string>
#include <chrono>
#include "jvar.h"

class Fjwt {
public:

    /* valid algorithms parameter
    //HMSC	RSA	ECDSA	PSS	EdDSA// <- type, names values below
    HS256	RS256	ES256	PS256	Ed25519
    HS384	RS384	ES384	PS384	Ed448
    HS512	RS512	ES512	PS512	
    */
    static std::string makeToken(
        std::chrono::time_point<std::chrono::high_resolution_clock> expiryTime, 
        std::string algorithm,
        std::string pub_key, 
        std::string priv_key, 
        std::string issuer,
        jvar &claims
        );
    static bool validateToken(std::string token, std::string pub_key, std::string issuer);
    static jvar getClaims(std::string token);

    static std::string sha256(const std::string str);
    static std::string sha512(const std::string str);

    static std::string generateRandomString(size_t size);
};

#endif