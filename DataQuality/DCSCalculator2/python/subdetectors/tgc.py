# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
The meaning of TGC flags:

        Currently, the colors refer to the percent of 'bad' hv channels.
        An hv channel is considered bad if its voltage that is less than 2775 volts:
          red:      >10%
          yellow:   >3% and <=10%
          green:    <=3%

        The formula is planned to be enhanced in the future.

TGC specific logic for calculating DQ flags:
  1) Conditions are not taken from COOL
        (As opposed to other subdetectors) TGC DQ flags are calculated based on data from TGC DB rather than COOL.
        The conditions reside at table ATLAS_CONF_TGC.TGC1CONDITION_DATA, at DB ATLAS_CONFIG.
        The query of conditions uses a view on this table: TGC1VCONDITION_DATA.
    Enhancement: Take current values from ATLAS_CONF_TGC.TGC1CURRENT_CONDITION if channel DATE is smaller than until
        Table TGC1LOCATION is queried in order map the identifiers of conditions to the DQ channel_id's.

  2) TGC conditions have one DATE column rather than iov_since and iov_until at COOL
        In addition, TGC conditions table has only one DATE column, as opposed to iov_since and iov_until 
        columns of COOL folders.
        Hence, the following code also calculated for each condition the equivalent to iov_until,
        as the DATE of next condition of the same channel, or, for the last conditions, the grand iov_until of the run.
"""

from __future__ import with_statement

from datetime import datetime, timedelta
from collections import namedtuple
from warnings import filterwarnings

from sqlalchemy import Numeric, create_engine, MetaData, Table, select
from sqlalchemy.sql import and_, not_, union

from DQUtils.general import timer
from DQUtils.sugar import IOVSet, TimestampType, define_iov_type
from DQUtils.oracle import make_oracle_connection

from DCSCalculator2 import config
from DCSCalculator2.lib import DCSC_DefectTranslate_Subdetector, DCSC_Variable, make_multi_mapping
from DCSCalculator2.consts import (
    BLACK, WHITE, GREY, RED, YELLOW, GREEN, EMPTY, OUT_OF_CONFIG, BAD, GOOD)

TGCEA, TGCEC = 324, 325

# 24/11/10 dfront: add optional logging for alchemy
do_log_alchemy=False
if do_log_alchemy:
    import logging
    logging.basicConfig()
    logging.getLogger('sqlalchemy.engine').setLevel(logging.INFO)
    logging.getLogger('sqlalchemy.orm.unitofwork').setLevel(logging.DEBUG)

filterwarnings("ignore", "Did not recognize type .*")

class TGC_Database(object):
    metadata = None

    def __init__(self):
        conn_str = "oracle://ATLAS_CONFIG/ATLAS_CONF_TGC_R"
        engine, self.metadata = make_oracle_connection(conn_str)
    def get_metadata(self):
        return self.metadata

    def print_metadata(self): 
        """
        Print list of tables of schema, for debugging
        """
        self.metadata.reflect(bind=engine, schema='ATLAS_CONF_TGC')
        for table in reversed(self.metadata.sorted_tables):
            print 'table:', table

@define_iov_type
def TGC_HV_VAL(channel, actual_vMon):
    "High voltage TGC value."

TGC_HV_Data = namedtuple("TGC_HV_Data", "time channel value")

class DCSC_Variable_TGC_HV(DCSC_Variable):

    def read(self, query_range, folder_base, folder_name):
        """
        Reads conditions from Oracle rather than COOL
        """
        # Query from 2 days before q_since to ensue that each channel has a 
        # reading
        since = query_range.since.date - timedelta(days=2) 
        until = query_range.until.date
        
        result = self.read_tgc_conditions(since, until)
        
        return result

    def map_date2since_and_until(self, conditions, final_until):
        """
        raw input like: [(datetime.datetime(2010, 8, 9, 16, 8, 34), 76546081, '7.9050002098083'), 
                         (datetime.datet ...
        in order to handle times of each channel correctly, groups conditions 
        per channel, then creates the entries per channel.
        Should be ordered by time. Order imposed at DB query
        output: [(since, until, channel, value), (since, until ..
        """
        datapoints_by_channel = {}
        for c in conditions:
            datapoints_by_channel.setdefault(c.channel, []).append(c)
        
        final_until = TimestampType.from_date(final_until)
        
        def make_iovs_from_points(points, channel):
            """
            Take a list of data points and return a list of IoVs.
            """
            last_point = TGC_HV_Data(final_until, channel, None)
            for first, second in zip(points, points[1:] + [last_point]):
                yield TGC_HV_VAL._make((first.time, second.time) + first[1:])
 
        results = IOVSet()
        for channel, datapoints in datapoints_by_channel.iteritems():
            for iov in make_iovs_from_points(datapoints, channel):
                results.add(*iov)

        results = results.solidify(TGC_HV_VAL)

        EMPTY_TGC_HV_VAL = TGC_HV_VAL._emptycls
        since = results.first.since
        unfilled_channels = set(self.all_channels) - set(datapoints_by_channel.keys())
        for channel in unfilled_channels:
            results.append(EMPTY_TGC_HV_VAL(since, final_until, channel, None))
            
        return results

    def read_tgc_conditions(self, since, until):
        """
        select value, "DATE", loc_id from tgc1vcondition_data 
            where date>=since and date<until and dyn='actual.vMon';
        """
        conditions_view = Table('TGC1VCONDITION_DATA', self.db.get_metadata(), 
                                autoload=True, schema='ATLAS_CONF_TGC')
        current_view = Table('TGC1VCURRENT_CONDITION', self.db.get_metadata(), 
                                autoload=True, schema='ATLAS_CONF_TGC')
 
        time = conditions_view.c.DATE.label("time")
        channel = conditions_view.c.loc_id.label("channel")
        value = conditions_view.c.value.label("value")
        is_vmon = conditions_view.c.dyn == 'actual.vMon'
        channel_cond= and_(conditions_view.c.loc.like('HV%CHAN%'), not_(conditions_view.c.logical_name.like('%SPARE%')))

        time_current = current_view.c.DATE.label("time")
        channel_current = current_view.c.loc_id.label("channel")
        value_current = current_view.c.value.label("value")
        is_vmon_current = current_view.c.dyn == 'actual.vMon'
        channel_cond_current= and_(current_view.c.loc.like('HV%CHAN%'), not_(current_view.c.logical_name.like('%SPARE%')))
                         
        # Channels that have current values from before since
        query1 = (select([time_current, channel_current, value_current])
                 .where(and_(time_current < since, is_vmon_current, channel_cond_current)))

        # For other channels, look 1 day before since
        query1_channels= (select([channel_current])
                 .where(and_(time_current < since, is_vmon_current, channel_cond_current)))
        query2= (select([time, channel, value])
                 .where(and_(
                       time < until
                     , time >= since
                     , is_vmon
                     , channel_cond
                     , not_(channel.in_(query1_channels)))))
        query = query1.union(query2).order_by("channel", "time")
     
        def fixup_data(data):
            time = TimestampType.from_date(data.time)
            return TGC_HV_Data(time, data.channel, float(data.value))
        
        with timer("Query oracle for TGC data"):
            results = query.execute()
        
        with timer("Fetch TGC information from database"):
            results = map(fixup_data, results.fetchall())
        
        with timer("Rewrite results in terms of IoVs"):
            results = self.map_date2since_and_until(results, until)
        
        return results

def get_mapping(db):
    """
    select oid, 324+side_number side_channel_id 
        from atlas_conf_tgc.tgc1location where name like 'HV%CHAN%';
    """
    loc_table = Table('TGC1LOCATION', db.get_metadata(), 
                      autoload=True, schema='ATLAS_CONF_TGC')
                     
    # 24/11/10 dfront: add condition not like '%SPARE%', to avoid the need for TGC_BAD_IDS and hence removed_ids
    # condition = loc_table.c.name.like('HV%CHAN%')    
    condition = and_(loc_table.c.name.like('HV%CHAN%'), not_(loc_table.c.logical_name.like('%SPARE%')))
    query = (
        select([(loc_table.c.side_number+324).label("dq_region"),
                loc_table.c.oid.label("offline_id")])
        .where(condition)
    )
    
    mapping = query.execute().fetchall()
    
    # Make a map {offline_id: [id1, id2, id3, ....]}
    mapping = make_multi_mapping((int(m.dq_region), int(m.offline_id)) 
                                 for m in mapping)

    # Remove values which are in TGC_BAD_IDS
    for dq_region in mapping:
        dq_region = int(dq_region)
        #removed_ids = TGC_BAD_IDS[dq_region]
        mapping[dq_region] = sorted(set(mapping[dq_region]) ) #- removed_ids)
    return mapping

def tgc_hv_good(iov):
    """
    From TGC CPP API: const float minGoodHv=2775.f;
    
    If there is no iov, return None to indicate unfilled
    """
    voltage = iov.actual_vMon
    return voltage >= 2775 if voltage is not None else None

class TGC(DCSC_DefectTranslate_Subdetector):

    # Unused
    folder_base = None
    
    def __init__(self, *args, **kwargs):
        kwargs['keep_dcsofl'] = True
        db = TGC_Database()
        self.mapping = get_mapping(db)
    
        super(TGC, self).__init__(*args, **kwargs)
        self.get_variable("HV").db = db
        channels_=[]
        for el in self.mapping.values():
            channels_.extend(el)
        self.get_variable("HV").all_channels = channels_

        self.translators = [TGC.color_to_defect_translator(flag, defect)
                            for flag, defect in ((TGCEA, 'MS_TGC_EA_STANDBY_HV'),
                                                 (TGCEC, 'MS_TGC_EC_STANDBY_HV'),
                                                 )]
        
    variables = [
        DCSC_Variable_TGC_HV("HV", tgc_hv_good),
    ]
    
    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1
    

