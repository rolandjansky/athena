#!/usr/bin/env python
import datetime, time
import uuid

def uuid_to_ts( myuuid ) :
    # See http://www.gossamer-threads.com/lists/python/dev/670680
    DTD_DELTA = ( datetime.datetime( *time.gmtime(0)[0:3] ) - 
                  datetime.datetime( 1582, 10, 15 ) )
    DTD_SECS_DELTA = DTD_DELTA.days * 86400 # 0x01b21dd213814000/1e7
    secs = myuuid.time / 1e7 
    secs_epoch = secs - DTD_SECS_DELTA 
    return datetime.datetime.fromtimestamp( secs_epoch )

def printuuid( myuuid ) :
    myUUID=uuid.UUID(myuuid)
    print 'UUID:',myUUID
    print 'UUID variant:',myUUID.variant
    print 'UUID version:',myUUID.version,'(%1x)'%(myUUID.version) 
    print 'UUID field#0 (time_low):             %12.8x (%12.8x)'\
          %( myUUID.fields[0], myUUID.time_low )
    print 'UUID field#1 (time_mid):             %12.4x (%12.4x)'\
          %( myUUID.fields[1], myUUID.time_mid )
    print 'UUID field#2 (time_hi_version):      %12.4x (%12.4x)'\
          %( myUUID.fields[2], myUUID.time_hi_version )
    print 'UUID field#3 (clock_seq_hi_variant): %12.2x (%12.2x)'\
          %( myUUID.fields[3], myUUID.clock_seq_hi_variant )
    print 'UUID field#4 (clock_seq_low):        %12.2x (%12.2x)'\
          %( myUUID.fields[4], myUUID.clock_seq_low )
    print 'UUID field#5 (node):                 %12.12x (%12.12x)'\
          %( myUUID.fields[5], myUUID.node )
    print 'UUID time (100ns intervals since 15 Oct 1582):', myUUID.time
    print 'UUID time (100ns intervals since 15 Oct 1582):', hex(myUUID.time)
    print 'UUID timestamp:', uuid_to_ts(myUUID)
    print '================================================'

# DDCCBBAA-FFEE-HHGG-IIJJ-KKLLMMNNOOPP to AABBCCDD-EEFF-GGHH-IIJJ-KKLLMMNNOOPP
def pool_to_uuid( mypool ) :
    print 'POOL guid:',mypool
    fields = mypool.split('-')
    if ( len(fields) != 5 ) : raise "Wrong #fields in "+myguid
    if ( len(fields[0]) != 8 ) : raise "Wrong #chars in field#0 "+fields[0]
    if ( len(fields[1]) != 4 ) : raise "Wrong #chars in field#1 "+fields[1]
    if ( len(fields[2]) != 4 ) : raise "Wrong #chars in field#2 "+fields[2]
    if ( len(fields[3]) != 4 ) : raise "Wrong #chars in field#3 "+fields[3]
    if ( len(fields[4]) != 12 ) : raise "Wrong #chars in field#4 "+fields[4]
    f0 = fields[0]
    f1 = fields[1]
    f2 = fields[2]
    f3 = fields[3]
    f4 = fields[4]
    myuuid = '%s%s%s%s-%s%s-%s%s-%s-%s'\
             %(f0[6:8],f0[4:6],f0[2:4],f0[0:2],\
               f1[2:4],f1[0:2],f2[2:4],f2[0:2],f3,f4)
    print 'UUID guid:',myuuid
    return myuuid

# DDCCBBAA-FFEE-HHGG-IIJJ-KKLLMMNNOOPP to AABBCCDD-EEFF-GGHH-0000-000000000000
def pool_to_uuid( mypool ) :
    print 'POOL guid:',mypool
    fields = mypool.split('-')
    if ( len(fields) != 5 ) : raise "Wrong #fields in "+myguid
    if ( len(fields[0]) != 8 ) : raise "Wrong #chars in field#0 "+fields[0]
    if ( len(fields[1]) != 4 ) : raise "Wrong #chars in field#1 "+fields[1]
    if ( len(fields[2]) != 4 ) : raise "Wrong #chars in field#2 "+fields[2]
    if ( len(fields[3]) != 4 ) : raise "Wrong #chars in field#3 "+fields[3]
    if ( len(fields[4]) != 12 ) : raise "Wrong #chars in field#4 "+fields[4]
    f0 = fields[0]
    f1 = fields[1]
    f2 = fields[2]
    f3 = fields[3]
    f4 = fields[4]
    myuuid = '%s%s%s%s-%s%s-%s%s-%s-%s'\
             %(f0[6:8],f0[4:6],f0[2:4],f0[0:2],\
               f1[2:4],f1[0:2],f2[2:4],f2[0:2],f3,f4)
    print 'UUID guid:',myuuid
    return myuuid

# AABBCCDD-EEFF-GHIJ-xxxx-xxxxxxxxxxxx to HIJEEFFAABBCCDD
def timorder_uuid( myuuid ) :
    fields = myuuid.split('-')
    if ( len(fields) != 5 ) : raise "Wrong #fields in "+myguid
    if ( len(fields[0]) != 8 ) : raise "Wrong #chars in field#0 "+fields[0]
    if ( len(fields[1]) != 4 ) : raise "Wrong #chars in field#1 "+fields[1]
    if ( len(fields[2]) != 4 ) : raise "Wrong #chars in field#2 "+fields[2]
    if ( len(fields[3]) != 4 ) : raise "Wrong #chars in field#3 "+fields[3]
    if ( len(fields[4]) != 12 ) : raise "Wrong #chars in field#4 "+fields[4]
    f0 = fields[0]
    f1 = fields[1]
    f2 = fields[2]
    f3 = fields[3]
    f4 = fields[4]
    myuuidto = '%s%s%s'%(f2[1:4],f1,f0)
    myUUID=uuid.UUID(myuuid)
    print 'UUID:',myUUID
    print 'UUID time (100ns intervals since 15 Oct 1582):', hex(myUUID.time)
    print 'UUID timeordered:',myuuidto    
    print '================================================'
    return myuuidto

# DDCCBBAA-FFEE-IJGH-xxxx-xxxxxxxxxxxx to HIJEEFFAABBCCDD
def timorder_pool( mypool ) :
    print 'POOL guid:',mypool
    fields = mypool.split('-')
    if ( len(fields) != 5 ) : raise "Wrong #fields in "+myguid
    if ( len(fields[0]) != 8 ) : raise "Wrong #chars in field#0 "+fields[0]
    if ( len(fields[1]) != 4 ) : raise "Wrong #chars in field#1 "+fields[1]
    if ( len(fields[2]) != 4 ) : raise "Wrong #chars in field#2 "+fields[2]
    if ( len(fields[3]) != 4 ) : raise "Wrong #chars in field#3 "+fields[3]
    if ( len(fields[4]) != 12 ) : raise "Wrong #chars in field#4 "+fields[4]
    f0 = fields[0]
    f1 = fields[1]
    f2 = fields[2]
    f3 = fields[3]
    f4 = fields[4]
    myuuidto = '%s%s%s%s%s%s%s%s'\
               %(f2[2:3],f2[0:2],f1[2:4],f1[0:2],\
                 f0[6:8],f0[4:6],f0[2:4],f0[0:2],)
    print 'UUID timeordered:',myuuidto    
    return myuuidto

# [avalassi@lxplus248 tcsh] ~ > date ; pool_gen_uuid ; uuidgen -t
# Fri May 21 23:50:25 CEST 2010
# A2E338DD-2265-DF11-AFE1-001E4F3E5C33
# dd3a5aac-6522-11df-82eb-001e4f3e5c33

guid1='dd3a5aac-6522-11df-82eb-001e4f3e5c33' # from uuidgen
guid2='A2E338DD-2265-DF11-AFE1-001E4F3E5C33' # from POOL
guid3='A2E338DD-2265-D011-AFE1-001E4F3E5C33' # POOL token for the year 1997
printuuid( guid1 )
timorder_uuid( guid1 )
printuuid(pool_to_uuid( guid2 ))
timorder_pool( guid2 )
printuuid(pool_to_uuid( guid3 ))

