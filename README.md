dumpable - Serialization without any serialization codes in C++
========

What is it?
-----------

With **dumpable**, you could..

  * serialize your custom *dumpable* struct **without any extra code** for serialize/deserialize
  * deserialize in **constant time** (just casting to T*)

*dumpable* struct is a struct that contains only members with following types: 
  * POD
  * **dstring**, **dvector**, **dmap**
  * another *dumpable* struct

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
  2. Fill data and dump to a file.

    ```cpp
    // building sample data
    classroom data;
    
    data.class_name = "1001";
    data.students.push_back(student(L"Alice",2));
    data.students.push_back(student(L"Bob",5));
    data.students.push_back(student(L"\ud55c\uae00",13));
    
    // dump to file
    ostringstream out;
    dumpable::write(data, out);
    
    FILE* fp = fopen("dumped.bin", "wb");
    size_t buffer_size = out.str().size();
    fwrite(&buffer_size, 1, sizeof(buffer_size), fp);
    fwrite(out.str().data(), buffer_size, 1, fp);
    fclose(fp);

    // Wait, where's serialization code?
    // You don't need it with dumpable!
    ```
    Or you can do additional tasks like compression, encryption, adding meta informations and so on.

  3. Read from the file and reconstruct original data.
  
    ```cpp

    FILE* fp = fopen("dumped.bin","rb");
    fread(&buffer_size, 1, sizeof(buffer_size), fp);
    char* buffer = new char[buffer_size];
    fread(buffer, buffer_size, 1, fp);

    const classroom* pClassRoom = dumpable::from_dumped_buffer<classroom>(buffer);

    // pClassRoom is valid; Play with pClassRoom.
    
    ...
      
    // if we clear chunk ...
    delete[] buffer;
    // now pClassRom is now invalid.
    ```
    Note: **dumpable::from\_dumped\_buffer** takes constant time.
      
See **simple\_example** from test.cpp for more detail.

When to use? (or Why do I develop this library?)
------------------------------------------------

I'm working at a video game company, so I want to optimize the loading speed of the game. **dumpable** eliminates time for reconstructing game data from binary to C++ struct.

It can also be used for serializing network packets. While `protobuf` is a very good tool for it, **dumpable** supports nested C++ structs.

Thready Safety
--------------

Currently only one thread can call **dumpable::write**. (It uses global variable. It can be change to thread_local.)  
**dumpable::from\_dumped\_buffer** is thread-safe.

Limitation
----------

You cannot use **dumpable** with struct having virtual functions.  
Modifying **dumpable** containers could be slow.  
Versioning and error detection are not supported;  
Calling **dumpable::from_dumped_buffer\<T\>** with a buffer created by an object of type **U** may crash the program.  

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
