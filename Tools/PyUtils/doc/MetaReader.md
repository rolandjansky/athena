# MetaReader

This utility extracts metadata from a given ATLAS file.
MetaReader can be run as a "standalone" file or	    as a module (library).
(last updated on 6.02.2018)
---

## 1. Standalone version
### How to use:
To view all available arguments run:
```bash
python meta-reader.py -h
```
to run it:
```bash
python meta-reader.py filename1 filename2
```
_The user can give as input a file or a list of files._
### Arguments

* #### -h, --help
    _Displays the help information._

    Example:
    ```bash
    python meta-reader.py -h
    ```
    
* #### -v, --verbose
    _Displays some extra information on the screen._

    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -v
    ```
    
* #### -o, --output
    _The user can specify the name of the output filename. By default, the output is written on the screen (stdout) in a prettier format for better readability._

    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -o outFileName.txt
    ```
    
* #### --json
    _Sets the output file format as `json`._

    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -o json.txt --json
    ```
    
* #### --indent
    _Sets the indent spaces in the output file either on screen (without `-o` flag) either on file (with `-o` flag). By default, it uses two spaces as indent._
    
    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -o json.txt --json --indent 4
    ```

* #### -f, --full
    _Retrieve the full set of metadata from a file. By default, this is set to False and from a given file only the following keys: 'file_guid', 'file_size', 'file_type', 'nentries'._

    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -f
    ```
    Output:
    ``` python
    {'AOD.05352803._000417.pool.root.1': 
        {
            'file_guid': '406B5C0F-5386-E511-A367-02163E00E173',
            'file_size': 1173306136,
            'file_type': 'POOL',
             'nentries': 1260
        }
    }
    ```
    
* #### -t, --type
    _The file type of the input filename. It is optional because ```file_type``` is determined automatically. If the user wants to specify the ```file_type``` he has two options: POOL or BS (BS stands for bytestream files: RAW or DRAW)._

    Example:
    ```bash
    python meta-reader.py AOD.05352803._000417.pool.root.1 -t POOL
    ```
---

## 2. Module Version
### How to use:

Include in your file the following statement:

```python
from PyUtils.MetaReader import read_metadata
```
The `MetaReader.py` must be in the same directory to be used in this way.
If the `MetaReader.py` is located in other directory you need to set the system path using the following command:

```python
sys.path.append('/path/to/MetaReader.py')
```



### Parameters

```python
def read_metadata(filename, file_type=None, full=False):
```

* #### -filenames
    _The name of input files. The user can provide a file or a list of files._


* #### -file_type
    _The file type of the input filename. The user has two options: POOL or BS (BS stands for bytestream files: RAW or DRAW)._


* #### full
    _By default `full=False` and returns a "lite" version of metadata with only this heys: 'file_guid', 'file_size', 'file_type', 'nentries'. 
    If `full=True` this will return all metadata associated with the filename.


    Example:
    ```python
    read_metadata('AOD.05352803._000417.pool.root.1', file_type=None, full=False)
    ```

---