      character*50  colorLookupTable,decayLookupTable
      character*100  eventDataFile

      parameter ( eventDataFile      = 'event_input.dat     ' )

      common /lookupTables/ colorLookupTable,decayLookupTable
      
      integer unitnumber
      common /unit/ unitnumber

