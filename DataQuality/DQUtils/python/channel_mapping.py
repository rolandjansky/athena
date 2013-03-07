# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


try:
    from CoolRunQuery.AtlRunQueryLookup import DQChannelDict
except ImportError:
    from warnings import warn
    warn("Failed to import DQChannelDict from CoolRunQuery. "
         "Can't perform channel conversions.")
    channel_mapping = {}
    channel_names = []
else:
    # Need to decouple ourselves from CRQ by making a copy, else bad things.
    channel_mapping = DQChannelDict.copy()
    channel_names = channel_mapping.keys()

cm_reversed = dict((value, key) for key, value in channel_mapping.iteritems())
channel_mapping.update(cm_reversed)

def convert_channel(name, want_id=True, channel_mapping=channel_mapping):
    """
    Given a channel, return a name detector or an integer (depending on want_id)
    """
    if isinstance(name, (int, long)):
        if name not in channel_mapping:
            raise RuntimeError, "ChannelID %r is not in channel_mapping" % name
        return name if want_id else channel_mapping[name]
        
    elif isinstance(name, str):
        if name not in channel_mapping:
            raise RuntimeError, "ChannelID %r is not in channel_mapping" % name
        return name if not want_id else channel_mapping[name]
        
    raise RuntimeError, ("I don't know how to convert %r into "
                         "a ChannelSelection" % name)

def list_to_channelselection(list_, convert_channel=convert_channel, 
                             as_list=False):
    """
    Given a list, return a channelselection.
    
    Does the hard work of merging together adjacent channel numbers. Will also
    convert channel names to numbers (if the folder is using DQ channels,
    otherwise will give an incorrect result or fail to convert the strings to
    channelids.)
    """
    from PyCool import cool
    if not list_ and as_list: return []
    if not list_: return cool.ChannelSelection()
    if set(map(type, list_)) != set([int]) and convert_channel:
        list_ = map(convert_channel, list_)
    
    list_.sort()
    
    start = None
    ranges = []
    for this, next in zip(list_, list_[1:] + [-1]):
        if start is None:
            start = this
        if next - this != 1:
            ranges.append((start, this))
            start = None
    
    if as_list:
        return ranges
    
    assert len(ranges) < 50, (
        "Cool has a 50 channel selection limit (%s selected)" % ranges)
    
    selection = cool.ChannelSelection(*ranges.pop(0))
    for start, end in ranges:
        selection.addRange(start, end)
        
    return selection
    
def make_channelselection(cs, mapping=None):
    """
    Helper function which can convert a channel id, name, or list of either into
    a cool.ChannelSelection. Includes protections for invalid channels.
    """
    from PyCool import cool
    if mapping == None:
        mapping = channel_mapping
    if cs is None or cs == []:
        return cool.ChannelSelection()
    elif isinstance(cs, (int, str)):
        channel_id = convert_channel(cs, channel_mapping=mapping)
        return cool.ChannelSelection(channel_id, channel_id)
    elif hasattr(cs, "__iter__"):
        cc = lambda cs: convert_channel(cs, channel_mapping=mapping)
        return list_to_channelselection(cs, convert_channel=cc)
    elif isinstance(cs, cool.ChannelSelection):
        return cs
    raise RuntimeError, ("I don't know how to convert %r into a "
                         "ChannelSelection" % cs)

def get_channel_ids_names(folder):
    """
    Given a `folder` name, retrieve a list of channel ids, names and return a
    dictionary which will translate in either direction.
    """
    channel_ids = list(folder.listChannels())
    channel_names = folder.listChannelsWithNames()
    channel_names = map(channel_names.__getitem__, channel_ids)
    channel_dict = dict(zip(channel_ids,   channel_names)
                       +zip(channel_names, channel_ids))
    return channel_ids, channel_names, channel_dict

