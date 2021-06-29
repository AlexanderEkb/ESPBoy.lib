#include "Storage.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STORAGE)
StorageClass Storage;
#endif

void StorageClass::begin() {
  uint8_t foo = 0;
  SPIFFS.begin();
  // SPIFFS.remove(SHORT_GAME_NAME);
  const bool exists = SPIFFS.exists(fileName);

  if(exists) {
    file = SPIFFS.open(fileName, "r+");
  } else {
    file = SPIFFS.open(fileName, "w+");
  }

  if(file) {
    if(!exists) {
      Serial.println("StorageClass create file");
      for(uint32_t index=0; index<fileSize; index++) {
        if(file.write(foo) != 1) {
          Serial.println("file write error");
        }
      }
      file.flush();
    } else {
      Serial.println("StorageClass: already exists");
    }

    char buf[64];
    const uint32_t file_size = file.size();
    sprintf(buf, "%d", file_size);
    Serial.println(buf);
  } else {
    Serial.println("StorageClass: file open error");
  }
}

uint8_t StorageClass::read(int const address) {
  Serial.println("StorageClass.read()");
  uint8_t result;
  if(file) {
    file.seek(address);
    if(file.read(&result, 1) != 1) {
      Serial.println("byte read error");
    }
  } else {
    result = 0;
  }

  return result;
}

void StorageClass::write(int const address, uint8_t const val) {
  Serial.println("StorageClass.write");
  if(file) {
    file.seek(address);
    if(file.write(val) != 1) {
      Serial.println("byte write error");
    }
  } 
}

bool StorageClass::commit() {
  return false;
}

void StorageClass::end() {

}

void StorageClass::update(int const address, uint8_t const val) {
  write(address, val);
}
