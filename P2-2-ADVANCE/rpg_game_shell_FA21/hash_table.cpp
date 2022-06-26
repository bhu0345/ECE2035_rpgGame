/*
 Student Name: Bingtao Hu
 Date: 11/4/2021

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
  // Allocate space for the new entry
  HashTableEntry* newItem = (HashTableEntry*)malloc(sizeof(HashTableEntry));

  if (newItem == NULL){
    printf("Error: Insufficient space");
    exit (1);
  }

  // initialize the new entries in the struct
  newItem->key = key;
  newItem->value = value;
  newItem->next = NULL;
  return newItem;
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
  // get the index of the bucket
  unsigned int index = hashTable->hash(key);

  // create a pointer cast to the head of the bucket
  HashTableEntry *node = hashTable->buckets[index]; 
  while (node != NULL && node->key != key) 
    node = node->next; 
  return node;
 
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}


void destroyHashTable(HashTable* hashTable) {

  for (int i=0; i < hashTable -> num_buckets; i++){
    HashTableEntry* node = hashTable->buckets[i]; // points to the head of the bucket
    // while node still exists
    while(node){ 
      HashTableEntry* previous = node; 
      node = node->next; // Get the next value 
      free(previous->value); 
      free(previous); 
    }
  }
    // destroy hash buckets and hash table itself
    free(hashTable->buckets); 
    free(hashTable); 
  }


void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  // find the item
  HashTableEntry *node = findItem(hashTable,key);
  // if the key exists, replace with the new value
  if(node){
    void* old = node->value;
    node->value = value;
    return old; 
  }
  // if the key does not exists, insert the new value 
  else {
    unsigned int index=hashTable->hash(key);
    HashTableEntry *newEntry = createHashTableEntry(key,value); // Create a new hash table entry
    newEntry->next = hashTable->buckets[index];
    hashTable->buckets[index] = newEntry;
    // Return NULL if the key does not exists
    return NULL;
  }
}

void* getItem(HashTable* hashTable, unsigned int key) {
  // find the item
   HashTableEntry *node = findItem(hashTable,key);
   if(node){
     return (node->value);
   }
   // If there isn't the item that we are looking for, return NULL
   else{
     return NULL;
   }
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  unsigned int index = hashTable->hash(key);
  HashTableEntry* node = hashTable->buckets[index];
   if (!node)
   // return NULL if the head entry does not exist
     return NULL;

  // if the value on the head is to be removed
  if (node->key ==key){
    void* temp = node->value;
    hashTable->buckets[index] = node->next;
    free(node);
    return temp;
     }
  // if the node is not on the head, loop through the list to find the node
  while (node->next) {
    if (node->next->key == key) {
      void* val= node->next->value;
      HashTableEntry* temp = node->next;
      node->next = node->next->next;
      free(temp);
      return val;
    }
    node = node->next;
  }
  return NULL;
   
}

void deleteItem(HashTable* hashTable, unsigned int key) {
  // remove and free the entry that was called
  free(removeItem(hashTable, key)); 
}
