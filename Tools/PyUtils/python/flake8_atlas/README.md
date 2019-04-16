# ATLAS plugins for flake8

This module contains ATLAS-specific `flake8_atlas` plugins. It is built by default in the
athena release and available after running `asetup`. To verify if the plugin is found
run:
```
> flake8 --version
3.6.0 (flake8_atlas: 1.0, mccabe: 0.6.1, pycodestyle: 2.4.0, pyflakes: 2.0.0) CPython 2.7.15 on Linux
```

Plugins with codes `>=ATL900` are disabled by default. To enable them use
```
flake8 --enable-extension=ATL901
```
In addition to ATLAS specific plugins most of the `python23.py` plugins from
the [hacking](https://github.com/openstack-dev/hacking) plugin by OpenStack
were imported (and some modified).