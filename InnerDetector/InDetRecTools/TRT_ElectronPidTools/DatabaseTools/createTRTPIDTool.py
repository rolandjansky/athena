# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__ = 'mborodin'

# python createTRTPIDTool.py infile.txt 'sqlite://;schema=TRTPID.db;dbname=CONDBR2'

import sys
import logging
from PyCool import cool

FOLDER_NAME = '/TRT/Calib/PID_vector'
TAG_NAME = 'TRTCalibPID_vector_Data15_noArCFs_noZR_00-01'
# python createTRTPIDTool.py CorrectionFactors_HRformat_Jared_Data.txt 'sqlite://;schema=TRTPIDData.db;dbname=CONDBR2'


#TAG_NAME = 'TRTCalibPID_vector_MC_OnSetMC_CorrMC_00-01'
# python createTRTPIDTool.py CorrectionFactors_HRformat_Jared_MC.txt          'sqlite://;schema=TRTPIDMC.db;dbname=CONDBR2'


logging.basicConfig(level=logging.DEBUG)
_logger = logging.getLogger('createPIDTool')





def store_PID_to_folder(db,values_list):
        """
        Function to create folder FOLDER_NAME in db with channels and input data from values_list with tag TAG_NAME
        :param db: Open cool db
        :param values_list: list of tuples ('channel name', valus array)

        """
        spec = cool.RecordSpecification()
        spec.extend('array_value',cool.StorageType.Float)
        _logger.info('Create folder %s' % FOLDER_NAME)
        folder_spec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION,
                                           spec,
                                           cool.PayloadMode.VECTORPAYLOAD)
        folder = db.createFolder(FOLDER_NAME, folder_spec,
                               ' <timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="55403898"/></addrHeader><typeName>CondAttrListVec</typeName>',
                               True)

        for index, channel_name in enumerate(values_list):
            folder.createChannel(index,channel_name[0])
            print '%i - %s' % (index, channel_name[0])
        folder.setupStorageBuffer()
        for index, channel_values in enumerate(values_list):
            _logger.info('Store data from %s' % channel_values[0])
            values = channel_values[1]
       #     print channel_values[0]
       #     print channel_values[1]
            vector = cool.IRecordVector()
            for value in values:
       #         print value
                data = cool.PyCool.Helpers.IRecordPtr(spec)
                data.get()['array_value'] = value
                vector.push_back(data)
            folder.storeObject(0,cool.ValidityKeyMax, vector, index, TAG_NAME )
        folder.flushStorageBuffer()


def read_PID_txt(input_file_name):
        """
        Read data from input txt file in awful format
        :param input_file_name: input file path
        """
        output_list=[]
        with open(input_file_name,'r') as input_file:
                for line in input_file:
#			print line
                        #line = line.replace(' ','').replace("\n",'')
                        line = line.replace("\n",'')
                        if line and (line.find('#')==-1):
                                #elements = line.split(',')
                                elements = line.split()
				print elements

                                if len(elements) > 5:
                                    #Why?!
                                    barrel_or_endcap = elements[2].replace('EndcapWheels','EW')
                                    electrons = elements[3].replace('Electrons','Elecs')
                                    ChannelName = elements[4] + '_' +elements[1] + '_' + elements[2] + '_' +elements[0] + '_' +  elements[3]
                                    #ChannelName = elements[4] + '_' + elements[2] + '_' +elements[1] + '_' +elements[0] + '_' +  elements[3]
                                    #output_list.append(((elements[1]+'_'+barrel_or_endcap+'_'+electrons),map(float,elements[8:])))
                                    print elements[0:4]
                                    print ChannelName
        #                            print elements[5:]
                                    output_list.append(( ChannelName ,map(float,elements[5:])))
                                else:
                                	print "SIZE OF THE LINE ODD. CHECK!!! "
                                	print line
        return output_list


def main():

        if len(sys.argv) != 3:
            print "Usage: %s input_file connection_string" % sys.argv[0]
            return -1
        values_dict = read_PID_txt(sys.argv[1])
	print "\nREAD OVER"
        connect_string = sys.argv[2]
        _logger.info("data from file: %s" % values_dict)
	#print values_dict
        try:
            dbSvc = cool.DatabaseSvcFactory.databaseService()
            _logger.debug('dropping database %s' % connect_string)
            dbSvc.dropDatabase( connect_string )
            _logger.debug('creating database')
            db = dbSvc.createDatabase( connect_string )
        except Exception,e:
            _logger.error("Problem with database: %s" % e)
            return -1
        store_PID_to_folder(db, values_dict)
        db.closeDatabase()

if __name__=='__main__':
        main()
