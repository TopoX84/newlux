#include "uint256.h"
#include <string.h>
#include "sync.h"
#include "rx2.h"
#include "RandomX/randomx.h"

static RecursiveMutex cs_randomx;


void rx_slow_hash(const char* data, char* hash, int length, uint256 seedhash)
{
    ENTER_CRITICAL_SECTION(cs_randomx) 

    static bool is_init; // = false;
//    static char randomx_seed[64]; //={0}; // this should be 64 and not 32
    static uint256 randomx_seed2;
    static randomx_flags flags;
    static randomx_vm *rx_vm; // = nullptr;
    static randomx_cache *cache; // = nullptr;

 
    if (!is_init) {

        randomx_seed2 = seedhash;
    if (!cache) {
        flags = randomx_get_flags();
        cache = randomx_alloc_cache(flags);
       
        randomx_init_cache(cache, randomx_seed2.GetHex().c_str(), randomx_seed2.GetHex().size());
    }
 
    if (!rx_vm)
        rx_vm = randomx_create_vm(flags, cache, nullptr);

    is_init = true;
    }
    if (randomx_seed2!=seedhash) {
        randomx_seed2 = seedhash;

    randomx_destroy_vm(rx_vm);
    randomx_release_cache(cache);

    cache = randomx_alloc_cache(flags);
    randomx_init_cache(cache, randomx_seed2.GetHex().c_str(), 64);
    rx_vm = randomx_create_vm(flags, cache, nullptr);

    }   
    randomx_calculate_hash(rx_vm, data, length, hash);

        LEAVE_CRITICAL_SECTION(cs_randomx) 
}


void rx_slow_hash2(const char* data, char* hash, int length, uint256 seedhash)
{
        ENTER_CRITICAL_SECTION(cs_randomx) 

    static bool is_init; // = false;
//    static char randomx_seed[64]; //={0}; // this should be 64 and not 32
    static uint256 randomx_seed2;
    static randomx_flags flags;
    static randomx_vm *rx_vm; // = nullptr;
    static randomx_cache *cache; // = nullptr;
    static char old_data[288];
    static char old_hash[64];

 
    if (!is_init) {
        randomx_seed2 = seedhash;
    if (!cache) {
        flags = randomx_get_flags();
        cache = randomx_alloc_cache(flags);
       
        randomx_init_cache(cache, randomx_seed2.GetHex().c_str(), randomx_seed2.GetHex().size());
        is_init = true;
    }
 
    if (!rx_vm)
        rx_vm = randomx_create_vm(flags, cache, nullptr);

//    is_init = true;
    }
    if (randomx_seed2!=seedhash) {
        randomx_seed2 = seedhash;
//        randomx_reinit();
    randomx_destroy_vm(rx_vm);
    randomx_release_cache(cache);

    cache = randomx_alloc_cache(flags);
    randomx_init_cache(cache, randomx_seed2.GetHex().c_str(), 64);
    rx_vm = randomx_create_vm(flags, cache, nullptr);
    

    }   

    if (memcmp(data,old_data,144)==0) {            
    memcpy(hash,old_hash,32);
    }
    else { 
    randomx_calculate_hash(rx_vm, data, length, hash);
    memcpy(old_data,data,144);
    memcpy(old_hash,hash,32);
    }
        LEAVE_CRITICAL_SECTION(cs_randomx) 
}


void rx_slow_hash2_old(const char* data, char* hash, int length, uint256 seedhash)
{

    bool is_init = false;
    printf("rx_slow_hash2_old\n");
//    static char randomx_seed[64]; //={0}; // this should be 64 and not 32
     uint256 randomx_seed2;
     
        randomx_seed2 = seedhash;
 
        randomx_flags flags = randomx_get_flags();
        randomx_cache * cache = randomx_alloc_cache(flags);
        randomx_init_cache(cache, randomx_seed2.GetHex().c_str(), 64);
        randomx_vm *rx_vm = randomx_create_vm(flags, cache, nullptr);

   
 
    randomx_calculate_hash(rx_vm, data, length, hash);


    randomx_destroy_vm(rx_vm);
    randomx_release_cache(cache);


}