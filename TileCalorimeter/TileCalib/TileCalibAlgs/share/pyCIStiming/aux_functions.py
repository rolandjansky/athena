
def checkPMTValid(ros, pmt):

	ros += 1
	pmt += 1
	
	chan2pmt = []
	if ros==3 or ros==4:
		chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  \
			  13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,  \
			  -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
			  44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
	elif ros==1 or ros==2:
		chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, \
			  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
			  27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34, \
			  39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46]
		
	if pmt in chan2pmt:
		return 'real'
	elif -1*pmt in chan2pmt:
		return 'noninst'
	else:
		print ros, pmt
		print "There is a problem with the indexing."
		return -1


def checkChannelValid(ros, mod, chan):
	ros += 1
        
        # ROS Format:
        chan2pmt = []
        if ros==3 or ros==4:
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  \
                      13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,  \
                      -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
                      44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
        elif ros==1 or ros==2:
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, \
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
                      27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34, \
                      39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46] 
                      
        if(ros < 0 or ros >= 5):
            return 'noninst'
        if(mod < 0 or mod >= 4):
            return 'noninst'
        if(chan < 0 or chan >= 48):
            return 'noninst'
        
        if len(chan2pmt) > chan and chan2pmt[chan] < 0:
            return 'noninst'
        
        return 'real'



def ChanneltoPMT(ros, channel):
	chan2pmt = []
	if ros==3 or ros==4:
		chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  \
			  13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,  \
			  -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
			  44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
	elif ros==1 or ros==2:
		chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, \
			  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
			  27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34, \
			  39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46]

	return chan2pmt[channel]
		

def PMTtoChannel(ros, pmt):

        ros += 1
        pmt += 1
        
        chan2pmt = []
        if ros==3 or ros==4:
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  \
                      13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,  \
                      -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
                      44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
        elif ros==1 or ros==2:
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, \
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
                      27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34, \
                      39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46] 

        if pmt in chan2pmt:
            return chan2pmt.index(pmt)
        elif -1*pmt in chan2pmt:
            return chan2pmt.index(-1*pmt)
        else:
            print ros, pmt
            print "There is a problem with the indexing."
            return -1

