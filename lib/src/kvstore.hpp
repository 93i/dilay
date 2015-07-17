#ifndef DILAY_KVSTORE
#define DILAY_KVSTORE

#include <string>
#include "macro.hpp"

class KVStore {
  public:   
    DECLARE_BIG2 (KVStore, const std::string&, bool, const std::string&)

    template <class T> const T& get (const std::string&) const;  
    template <class T> const T& get (const std::string&, const T&) const;  
    template <class T> void     set (const std::string&, const T&);  

    void writeToDisk (bool) const;

  private:
    IMPLEMENTATION
};

class KVStoreProxy {
  public:
    KVStoreProxy (KVStore& s, const std::string& p) 
      : store  (s)
      , prefix (p) 
    {
      assert (p.back () == '/');
    }

    KVStoreProxy (KVStoreProxy& s, const std::string& p) 
      : KVStoreProxy (s.store, s.prefix + p)
    {}

    std::string key (const std::string& p) const {
      return this->prefix + p;
    }

    template <class T> 
    const T& get (const std::string& p) const { 
      return this->store.get <T> (this->key(p));
    }

    template <class T> 
    const T& get (const std::string& p, const T& v) const {
      return this->store.get <T> (this->key (p), v);
    }

    template <class T> 
    void set (const std::string& p, const T& v) const {
      return this->store.set <T> (this->key (p), v);
    }

  private:
    KVStore&          store;
    const std::string prefix;
};

#endif
