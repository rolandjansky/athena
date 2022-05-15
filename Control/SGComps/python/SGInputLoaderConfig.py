#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SGInputLoaderCfg(flags, Load=None, **kwargs):
    if Load:
        processed = []
        for item in Load:
            if isinstance(item, tuple):
                type_name, key = item
            elif isinstance(item, str):
                if '/' in item:
                    type_name, key = item.split('/')
                elif '#' in item:
                    type_name, key = item.split('#')
                else:
                    raise ValueError("String entry should be a type-key pair split by '/' or '#'")
            else:
                raise ValueError('Unsupported type')

            # Append 'StoreGateSvc' by default
            if '+' not in key:
                key = f'StoreGateSvc+{key}'

            processed.append((type_name, key))

        kwargs.setdefault('Load', processed)

    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.SGInputLoader(**kwargs), primary=True)
    return acc
