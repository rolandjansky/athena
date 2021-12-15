# TestTools

Tools and script used for (unit) testing. This package should have very minimal external
dependencies as it is included in all projects.

## Post-processing scripts
The following scripts can be used to customize the post-processing in [`atlas_add_test`](https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/SoftwareDevelopmentWorkBookCMakeInAtlas#atlas_add_test), e.g.:
```cmake
atlas_add_test( ...
   POST_EXEC_SCRIPT <script>
```
where `<script>` is one of:
- [`post.sh`](share/post.sh): **default** post-processing script used for comparing the test log file
  against a reference
- [`nopost.sh`](share/nopost.sh): no post-processing
- [`noerror.sh`](share/noerror.sh): check log file for common error patterns
- any user-defined script
