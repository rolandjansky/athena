HDF5 Utilities
==============

This package contains several tools, both to convert (simple) ROOT
files to HDF5 and to simplify writing HDF5 directly from jobs.


Root to HDF5 Converter
----------------------

Run `ttree2hdf5` to convert your root tree to an HDF5 file.

Branches that contain basic types (one value per entry in the tree)
are stored as a 1D array of compound data type. Branches that store
`vector<T>` are stored in a 2D array, while branches of type
`vector<vector<T> >` are stored in a 3D array. If you want more
dimensions feel free to [file an issue][1] with the original project
(which is hosted in github).

Note that **we only support the types listed above**. The purpose of
this package is _not_ to provide a generic converter from ROOT to
HDF5, but rather to convert simple ROOT files to a more widely
supported data format.

### Output Format ###

You can specify the maximum dimensions of the HDF5 array with

```
ttree2hdf5 <root-file> -o <output-file> -l [dims..]
```

where the `[dims..]` argument can have up to two integers.

You can also filter branches with the `--branch-regex` option, or
apply a selection (via TCuts) with `--selection`.

For more options check `ttree2hdf5 -h`.


Writing Directly From Jobs
--------------------------

This package provides a wrapper on the HDF5 C++ bindings to simplify
dumping data. It makes no attempt to replicate the element links or
other advanced features of the xAODs: the focus is primary to provide
something close to the final format for training machine learning
algorithms.

To dump some jet information, for example, first set up
the output file:

```C++
H5::H5File file("name.h5", H5F_ACC_TRUNC);
```

Next we need to set up a list of "consumers" which will translate
your objects into the output data:

```C++
H5Utils::Consumers<const xAOD::Jet*> cons;
cons.add("pt", [](const xAOD::Jet* j){ return j->pt();}, NAN);
cons.add("index", [](const xAOD::Jet* j){ return j->index();}, -1);
```

The `add(...)` method has signature

```C++
Consumers<C>::add(string name, function<S(C)>, S default_value);
```

where `C` is the type to be consumed (i.e. `xAOD::Jet`) and `S` is the
type which will be stored in the output file. In general the type of
`S` can be inferred from the function (we currently support most types
of `float` and `int`, plus `bool`). The `default_value` will be
explained below.

Next you need to set up the writer itself:

```C++
H5Utils::Writer<1,const xAOD::Jet*> writer(file, "jets", cons, {{5}});
```

The writer has a signature

```C++
Writer<N,C>::Writer(Group, string name, Consumers, Array shape, size_t buffer);
```

where `N` 1 in the case above because we want to write out a rank 1
block in each event (5 jet). In general this can be any rank (think
images, or tracks associated to jets) and the rank of the output
dataset will be `N` + 1, since one row is added per event.

The `Group` can be an `H5File` or a subgroup. A dataset named `name`
will be created in this group.

The `shape` argument is an array, which must be of rank `N`. HDF5
datasets are n-dimensional, but the shape must be a hypercube. In the
case above, we're going to create a M x N block, where M is the number
of entries. In cases where we have more than N jets, this means we'll
have to truncate them, whereas in cases where we have fewer than N
we'll pad the output dataset with `default_value`.

You can also specify the `buffer` size, which is the number of times
that `fill` is called before the writer flushes to file. This has a
default value of a few thousand, but you can tweak it if performance
is a problem.

Finally, we have to fill the dataset, which we'll do in your event
loop:

```C++
const xAOD::JetContainer *jets = 0;
event.retrieve(jets, "AntiKtWhoCaresJets");
writer.fill(*jets);
```

That's all! Note that `fill` expects a rank 1 array here, meaning that
any standard type (`vector`, `map`, `list`, etc) will work. If you
want to set `N` larger than 1, you'll have to pass in nested arrays of
the form `vector<vector<T> >`.

To be safe, you should probably call `writer.flush()` to
ensure that all jets are written before exiting the job, but this will
also be handled by the destructor.

### Writing multiple types of objects ###

This package doesn't provide a convenient way to store multiple
variable-length object arrays such that they are aligned by index. You
are free to create several writers, each of which will write a dataset
corresponding to a different type of object. These can write to the
same file, but aligning the indices after the fact is up to you!

### Lossy Compression

Many machine learning frameworks use 'half-precision' (16 bit)
floats. This means you probably won't gain much by saving data as 32
bit `float` or, much less, 64 bit `double`. By default atomic types
are saved at their native precision, but if you want to reduce this
you can specify a `Compression`:

```C++
consumers.add(name, function, default_value, COMPRESSION);
```

Currently we support:
 - `STANDARD`: use standard native precision
 - `HALF_PRECISION`: 16 bit

Merging files
-------------

This package includes `hdf5-merge` to merge HDF5 datasets. By default
it will merge along the first axis.

**Note, when merging on the grid**, you should use `hdf5-merge-nolock`
to avoid HDF5's check on file locking. Some grid sites use file
systems that don't support file locking. While we don't need file
locking, HDF5 will fail if you don't use this wrapper on thse sites.


Hacking This Code
=================

I've _tried_ to make this code as modular and straightforward as
possible. Of course if anything is unclear you should
[file an issue][1] or a [jira ticket][1j].

Repository Layout
-----------------

The user facing code is in a few headers in `HDF5Utils`:

   - `Writer.h`: defines `Consumers` and `Writer`. Use this.
   - `HdfTuple.h`: older interface to write hdf5 files, not recommended
   - `H5Traits.h`: structures to add type safety. Not for users.
   - `common.h`: utility functions, also not for users.

We don't recommend that you touch anything in the `internal`
namespace: these things are only in headers because the code uses lots
of templates.

### Utilities ###

The utility `ttree2hdf5` is a thin wrapper on `copyRootTree`. Reading
the code in `src/copyRootTree.cxx` should be a decent introduction to
the higher level classes you'll need to interact with.

To Do
-----

Other things I might do sometime:

 - Allow grouping of different branches into different output datasets
 - Merge with the other [root to hdf5 converter][2] I made for histograms.

Versions of this code
=====================

This code was originally a non-ATLAS project [hosted on github][3]. It
wasn't sufficiently complicated to merit including as an external
package, and some modifications were required to build in the ATLAS
environment. As such the two projects may diverge.

[1]: https://github.com/dguest/ttree2hdf5/issues
[1j]: https://its.cern.ch/jira/projects/ATLASG/
[2]: https://github.com/dguest/th2hdf5
[3]: https://github.com/dguest/ttree2hdf5

