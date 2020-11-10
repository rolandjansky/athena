lwtnn Utilities
===============

This is some Atlas-side helpers for the [lwtnn][1] package.

At some point things here might be migrated upstream, but we keep this
package around for development that might not want to wait for
`AtlasExternals` to be updated.

Package Contents
----------------

- `FastGraph`: Faster version of `lwt::LightweightGraph`. Takes
  vectors rather than `std::map<std::string,...>` inputs.


[1]: https://www.github.com/lwtnn/lwtnn
