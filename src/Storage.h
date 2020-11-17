#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "EEPROM.h"
#include "FS.h"
#include "../../src/espboy_game_desc.h"

class StorageClass {
public:
  StorageClass(void) : fileName(SHORT_GAME_NAME), fileSize(STORAGE_CAPACITY) {};

  void begin();
  uint8_t read(int const address);
  void write(int const address, uint8_t const val);
  bool commit();
  void end();

  uint8_t * getDataPtr() {return NULL;};
  uint8_t const * getConstDataPtr() const {return 0;};
  void update(int const address, uint8_t const val); 

  template<typename T> 
  T &get(int const address, T &t) {
    return t;
  }

  template<typename T> 
  const T &put(int const address, const T &t) {
    return t;
  }

  size_t length() {return _size;}

  uint8_t& operator[](int const address) {return getDataPtr()[address];}
  uint8_t const & operator[](int const address) const {return getConstDataPtr()[address];}

protected:
  File file;
  char const * const fileName;
  uint32_t const fileSize;
  uint32_t _sector;
  uint8_t* _data;
  size_t _size;
  bool _dirty;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STORAGE)
extern StorageClass Storage;
#endif

#endif /* _STORAGE_H_ */