**DocSearchEngine** is a multithreaded, inverted index based search utility meant for substring search within a text-based document collection, with further development in mind.

## Usage
To run DSE, open **`DocSearchEngine.exe`**. Input and output is currently handled through three .json files:
- **```config.json```** holds basic configuration (display name, maximum responses per request number, etc.) and defines the document database location(s) - both folder and file paths are supported; the current implementation processes **.txt and .md files only**. Additionally, the "version" field keeps track of the config format version and shouldn't be edited in most cases;
- **```requests.json```** holds the search requests, which will be used by the program to sift through the documents specified in *config.json*. A request may contain multiple words - the current implementation will try to find documents that contain the full query;
- **```answers.json```** holds the serialized program output - the queries' results are displayed in order, and all results per one query are printed out from most to least relevant.

## Building
#### Requirements
- C++20 compiler
- CMake 3.10+
#### Build
```
git clone https://github.com/alnazare/docsearchengine.git
cd docsearchengine
```
```
mkdir build
cd build
cmake ..
cmake --build
```