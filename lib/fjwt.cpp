#include "fjwt.h"
#include "jwt.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "base.h"
#include "ParallelLogger.hpp"

/* valid algorithms parameter
//HMSC 	                RSA	    ECDSA	PSS	    EdDSA// <- type, names values below
( deprecated, unsafe)
HS256	                RS256	ES256	PS256	Ed25519
HS384	                RS384	ES384	PS384	Ed448
HS512	                RS512	ES512	PS512	
*/
std::string Fjwt::makeToken(
        std::chrono::time_point<std::chrono::high_resolution_clock> expiryTime, 
        std::string algorithm,
        std::string pub_key, 
        std::string priv_key, 
        std::string issuer,
        jvar &claims
        )
{
    jwt::picojson_traits::string_type token="";
    /* REMOVING THIS OPTIONS AS THEY POSE A SECURITY RISK
    //https://connect2id.com/products/nimbus-jose-jwt/vulnerabilities
    if (algorithm=="HS256")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::hs256(pub_key,priv_key,"",""));
    }
    if (algorithm=="HS384")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::hs384(pub_key,priv_key,"",""));
    }
    if (algorithm=="HS512")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::hs512(pub_key,priv_key,"",""));
    }
    */
   try{
    if (algorithm=="RS256")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::rs256(pub_key,priv_key,"",""));
    }
    if (algorithm=="RS384")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::rs384(pub_key,priv_key,"",""));
    }
    if (algorithm=="RS512")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::rs512(pub_key,priv_key,"",""));
    }
    if (algorithm=="ES256")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::es256(pub_key,priv_key,"",""));
    }
    if (algorithm=="ES384")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::es384(pub_key,priv_key,"",""));
    }
    if (algorithm=="ES512")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::es512(pub_key,priv_key,"",""));
    }
    if (algorithm=="PS256")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::ps256(pub_key,priv_key,"",""));
    }
    if (algorithm=="PS384")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::ps384(pub_key,priv_key,"",""));
    }
    if (algorithm=="PS512")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::ps512(pub_key,priv_key,"",""));
    }
    if (algorithm=="Ed25519")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::ed25519(pub_key,priv_key,"",""));
    }
    if (algorithm=="Ed448")
    {
        token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWT")
        .set_expires_at(expiryTime)
        .set_issued_at(std::chrono::system_clock::now())
        .set_payload_claim("data", jwt::claim(claims.toJson()))
        .sign(jwt::algorithm::ed448(pub_key,priv_key,"",""));
    }
   }catch(std::exception &e)
   {
       logerror("FJWT Create Token:",e.what());
   }
    return token;
}

bool Fjwt::validateToken(std::string token, std::string pub_key, std::string issuer)
{
    jvar ret;
    try{
        auto decoded = jwt::decode(token);
        std::string algorithm = decoded.get_algorithm();
        jwt::verifier<jwt::default_clock,jwt::picojson_traits> verifier(jwt::default_clock{});
        bool verified = false;
    /*REMOVING THIS OPTIONS AS THEY POSE A SECURITY RISK
    //https://connect2id.com/products/nimbus-jose-jwt/vulnerabilities
    if (algorithm=="HS256")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::hs256(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="HS384")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::hs384(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="HS512")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::hs512(pub_key,"","",""));
        verified=true;
    }
    */
    if (algorithm=="RS256")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::rs256(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="RS384")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::rs384(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="RS512")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::rs512(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="ES256")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::es256(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="ES384")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::es384(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="ES512")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::es512(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="PS256")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::ps256(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="PS384")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::ps384(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="PS512")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::ps512(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="Ed25519")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::ed25519(pub_key,"","",""));
        verified=true;
    }
    if (algorithm=="Ed448")
    {
        verifier = jwt::verify()
        .with_issuer(issuer)
        .allow_algorithm(jwt::algorithm::ed448(pub_key,"","",""));
        verified=true;
    }
    if (!verified)
        return false;
    verifier.verify(decoded);

    return true;
    }catch(std::exception &e)
    {
        logerror("FJWT Validate Token:",e.what());
        return false;
    }
    return false;
}

jvar Fjwt::getClaims(std::string token)
{
    jvar ret;
    try{
        auto decoded = jwt::decode(token);
        ret["ok"] = true;
        for(auto& e : decoded.get_payload_claims())
        {
            if (e.first=="data")
            {
                ret= jvar::parseJson(e.second.as_string());
            }
            
        }
    }catch(std::exception &e)
    {
        logerror("FJWT Get Claims:",e.what());
        ret = jo {};
    }
    return ret;
}




// make sha256



std::string Fjwt::sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string Fjwt::sha512(const std::string str)
{
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, str.c_str(), str.size());
    SHA512_Final(hash, &sha512);
    std::stringstream ss;
    for(int i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

static char hexconvtab[] = "0123456789abcdef";

static char* php_bin2hex(const unsigned char *old, const size_t oldlen)
{
    char *result = (char*) malloc(oldlen * 2 + 1);
    size_t i, j;

    for (i = j = 0; i < oldlen; i++) {
        result[j++] = hexconvtab[old[i] >> 4];
        result[j++] = hexconvtab[old[i] & 15];
    }
    result[j] = '\0';
    return result;
}


 std::string Fjwt::generateRandomString(size_t desiredSize)
 {
     size_t size = desiredSize/2;
     unsigned char * buffer = new unsigned char[size];
     RAND_bytes(buffer,size);
    char *result = new char[size * 2 + 1];
    size_t i, j;
    for (i = j = 0; i < size; i++) {
        result[j++] = hexconvtab[buffer[i] >> 4];
        result[j++] = hexconvtab[buffer[i] & 15];
    }
    result[j] = '\0';

     std::string ret = result;
     delete[] buffer;
     delete[] result;
     return ret;
 }



//RAND_bytes


