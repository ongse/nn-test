typedef unsigned int uint32_t;

#ifndef POLARSSL_CONFIG_H
#define POLARSSL_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define POLARSSL_HAVE_LONGLONG

#define POLARSSL_HAVE_ASM

/* \} name */

#define POLARSSL_CIPHER_MODE_CFB

#define POLARSSL_CIPHER_MODE_CTR

#define POLARSSL_ERROR_STRERROR_DUMMY

#define POLARSSL_GENPRIME

#define POLARSSL_FS_IO

#define POLARSSL_PKCS1_V21

#define POLARSSL_SELF_TEST

#define POLARSSL_SSL_ALERT_MESSAGES

#define POLARSSL_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO

/* \} name */

#define POLARSSL_AES_C

#define POLARSSL_ARC4_C

#define POLARSSL_ASN1_PARSE_C

#define POLARSSL_ASN1_WRITE_C

#define POLARSSL_BASE64_C

#define POLARSSL_BIGNUM_C

#define POLARSSL_BLOWFISH_C

#define POLARSSL_CAMELLIA_C

#define POLARSSL_CERTS_C

#define POLARSSL_CIPHER_C

#define POLARSSL_CTR_DRBG_C

#define POLARSSL_DEBUG_C

#define POLARSSL_DES_C

#define POLARSSL_DHM_C

#define POLARSSL_ENTROPY_C

#define POLARSSL_ERROR_C

#define POLARSSL_GCM_C

#define POLARSSL_MD_C

#define POLARSSL_MD5_C

#define POLARSSL_NET_C

//#define POLARSSL_PADLOCK_C

#define POLARSSL_PEM_C

#define POLARSSL_PKCS5_C

#define POLARSSL_PKCS12_C

#define POLARSSL_RSA_C

#define POLARSSL_SHA1_C

#define POLARSSL_SHA2_C

#define POLARSSL_SHA4_C

#define POLARSSL_SSL_CACHE_C

#define POLARSSL_SSL_CLI_C

#define POLARSSL_SSL_SRV_C

#define POLARSSL_SSL_TLS_C

#define POLARSSL_TIMING_C

#define POLARSSL_VERSION_C

#define POLARSSL_X509_PARSE_C

#define POLARSSL_X509_WRITE_C

#define POLARSSL_XTEA_C
/* \} name */

//#define POLARSSL_CONFIG_OPTIONS   /**< Enable config.h module value configuration */

#if defined(POLARSSL_CONFIG_OPTIONS)

// MPI / BIGNUM options
//
#define POLARSSL_MPI_WINDOW_SIZE            6 
#define POLARSSL_MPI_MAX_SIZE             512 
// CTR_DRBG options
//
#define CTR_DRBG_ENTROPY_LEN               48 
#define CTR_DRBG_RESEED_INTERVAL        10000 
#define CTR_DRBG_MAX_INPUT                256 
#define CTR_DRBG_MAX_REQUEST             1024 
#define CTR_DRBG_MAX_SEED_INPUT           384 
// Entropy options
//
#define ENTROPY_MAX_SOURCES                20 
#define ENTROPY_MAX_GATHER                128 
// SSL Cache options
//
#define SSL_CACHE_DEFAULT_TIMEOUT       86400 
#define SSL_CACHE_DEFAULT_MAX_ENTRIES      50 
// SSL options
//
#define SSL_MAX_CONTENT_LEN             16384 
#endif /* POLARSSL_CONFIG_OPTIONS */

/* \} name */
#endif /* config.h */
