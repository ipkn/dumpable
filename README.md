Dumpable - C++ memcpy-able containers
========

What is it?
-----------
**Dumpable** is a set of containers that can be reconstructed from buffer in constant time.  
It can be used in combination with POD types and structures.  
You don't need to write serialization code for your struct; it just works magically!

Supported containers:

  - **string**
  - **vector**
  - **map**

These containers can be used like STL containers.

Example
-------

Extracted from **simple\_example** (test.cpp)

  1. Define custom struct
    - Use **dstring**, **dwstring**, **dvector**, **dmap** instead of **string**, **wstring**, **vector**, **map** respectively.
    
    ```cpp
    struct student
    {
      dwstring name;
      int score;
      student(){}
      student(const wstring& name, int score) : name(name), score(score) {}
    };

    struct classroom
    {
      dstring class_name;
      dvector<student> students;
    };
    ```
  2. Fill data and dump to stream.

    ```cpp
    // building sample data
    classroom data;
    
    data.class_name = "1001";
    data.students.push_back(student(L"Alice",2));
    data.students.push_back(student(L"Bob",5));
    data.students.push_back(student(L"\ud55c\uae00",13));
    
    // dump to file
    ofstream out("dumped.bin");
    dumpable::write(data, out);
    
    // Wait, where's serialization code?
    // You don't need it with dumpable!
    ```
    Or you can use ostringstream and do additional tasks like compression, encryption, adding meta informations and so on.

  3. Read from stream and reconstruct original data.
  
      ```cpp
      ifstream in("dumped.bin");
      string chunk(istreambuf_iterator(in), istreambuf_iterator());
      const classroom* pClassRoom = dumpable::from_dumped_buffer<const classroom>(chunk.data());

      // pClassRoom is valid; Play with pClassRoom.
      
      ...
      
      // if we clear chunk ...
      chunk.clear();
      // now pClassRom is now invalid.
      ```
      Note: **dumpable::from\_dumped\_buffer** takes constant time.
      
See **simple\_example** from test.cpp for more detail.

Thready Safety
--------------

Only one thread can call **dumpable::write**. (It uses global variable.)  
**dumpable::from\_dumped\_buffer** is thread-safe.

Limitation
----------

You cannot use **dumpable** with virtual functions.  
Modifying **dumpable** containers is slow.

Currently only few member functions are implemented. 

<!--**dmap::insert** is O(N) time operation.-->

Installation
------------

Dumpable is header-only library. Include "dumpable.h" and that's all!

License
-------

Dumpable is licensed under the MIT license.
See the file LICENSE.

Special thanks to
-----------------

Inspired by innover.
