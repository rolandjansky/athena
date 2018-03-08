# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl LHEUtils
#  PowhegControl Generic LHE file utilities
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import glob
import mmap
from AthenaCommon.Logging import logging
from xml.etree import ElementTree

# Initialise logging handler
logger = logging.getLogger("PowhegControl")


# Merge many input LHE files into a single output file
def merge(input_file_pattern, output_file):
    # Create input file list from pattern, ensuring that output_file is not in the list
    input_file_list = sorted([x for x in glob.glob(input_file_pattern) if not x == output_file])
    if len(input_file_list) < 1:
        raise IOError("No input LHE files provided. Aborting merge!")
    logger.info("Preparing to create {} from {} input files".format(output_file, len(input_file_list)))

    # Open output file and write events
    with open(output_file, "ab") as f_output:
        logger.info("... reading metadata from {}".format(input_file_list[0]))
        # Start with the first file and extract opening/closing string
        opening_string = get_opening_string(input_file_list[0])
        closing_string = get_closing_string(input_file_list[0])
        f_output.write(opening_string)

        # Now write events from all files to output
        # Use sanitised list in case output_file matches the pattern
        for nEvents, event in enumerate(event_iterator(input_file_list)):
            f_output.write(event)

        # Finally add the footer
        f_output.write(closing_string)

    # Write the event count to the logger
    logger.info("Wrote {} events to {}".format(nEvents + 1, output_file))


# Python generator to iterate through events from input LHE files
def event_iterator(input_files, verbose=True):
    # If a glob-able pattern is provided, expand this into a list
    if not isinstance(input_files, list):
        input_files = sorted(glob.glob(input_files))

    # Iterate over input files
    for file_name in input_files:
        if verbose:
            logger.info("... reading events from {}".format(file_name))
        in_event, event_lines = False, ""
        # Group all lines inside an XML event element
        with open(file_name, "rb") as f_input:
            for line in f_input:
                if "<event>" in line:
                    in_event = True
                    line = line[line.index("<event>"):]  # catch cases like "</init><event>"
                if in_event:
                    event_lines += line
                if "</event>" in line:
                    yield event_lines
                    in_event, event_lines = False, ""


# Python generator to iterate through event weights from input LHE files
def event_weight_iterator(input_files):
    for event in event_iterator(input_files):
        yield float([w for w in event.splitlines()[1].split(" ") if w][2])


# Count total number of events in input files
def event_counter(input_file_pattern):
    for nEvents, _ in enumerate(event_iterator(input_file_pattern, verbose=False)):
        pass
    return nEvents + 1


# Add a weight to a header passed as input (can be a string or an ElementTree)
def add_weight_to_header(header, weightgroup_name, weight_name, weight_id):
    # Convert string to ElementTree
    if not isinstance(header, ElementTree.Element):
        header_elem = ElementTree.fromstring(header)
    else:
        header_elem = header

    # Add initrwgt element if it doesn't exist
    if header_elem.find("initrwgt") is None:
        header_elem.append(ElementTree.fromstring("<initrwgt></initrwgt>"))

    # Add weightgroup element if it doesn't exist
    if not weightgroup_name in [x.attrib["name"] for x in header_elem.find("initrwgt").findall("weightgroup")]:
        header_elem.find("initrwgt").append(ElementTree.fromstring("<weightgroup combine='none' name='{}'></weightgroup>".format(weightgroup_name)))

    # Add weight to appropriate weightgroup
    weightgroup = [x for x in header_elem.find("initrwgt").findall("weightgroup") if x.attrib["name"] == weightgroup_name][0]
    if not any([weight.attrib["id"] == weight_id for weight in weightgroup.findall("weight")]):
        weightgroup.append(ElementTree.fromstring("<weight id='{}'> {} </weight>".format(weight_id, weight_name)))

    reindent_XML(header_elem)
    return header_elem


# Get opening lines from file as a string
def get_opening_string(input_LHE_file):
    with open(input_LHE_file, "rb") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        s_output = s_input[: s_input.find("<event>")]
    return s_output


# Get first event from file as a string
def get_first_event(input_LHE_file):
    with open(input_LHE_file, "rb") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        s_output = s_input[s_input.find("<event>"): s_input.find("</event>") + 8]
    return s_output


# Get closing lines from file as a string
def get_closing_string(input_LHE_file):
    with open(input_LHE_file, "rb") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        s_output = s_input[s_input.rfind("</event>") + 9:]
    return s_output


# Get weight name/value pairs from an input string
def string_to_weight(input_event):
    comment_lines = input_event[input_event.find("#"):].replace("\n", " ").replace("</event>", "")
    weight_lines = [" ".join(line.split()) for line in comment_lines.split("#") if "new weight,renfact,facfact,pdf1,pdf2" in line]
    return [(line.split(" ")[-1], line.split(" ")[2]) for line in weight_lines]


# Get new-style event weights from an input event string
def Powheg2LHEv3(input_event, name_to_ID):
    # Return event as-is if there are no Powheg-style weights
    if not "#new weight,renfact,facfact,pdf1,pdf2" in input_event:
        return input_event
    # Otherwise convert these to LHEv3 weights
    if not "<rwgt>" in input_event:
        nominal_weight = [x for x in input_event.split("\n")[1].split(" ") if len(x) > 0][2]
        input_event = input_event[:input_event.find("#")] + "<rwgt>\n<wgt id='0'> {0} </wgt>\n</rwgt>\n".format(nominal_weight) + input_event[input_event.find("#"):]
    weight_lines = "".join(["<wgt id='{}'> {} </wgt>\n".format(name_to_ID[weight[0]], weight[1]) for weight in string_to_weight(input_event)])
    return input_event[:input_event.find("</rwgt>")] + weight_lines + input_event[input_event.find("</rwgt>"):input_event.find("#new weight,renfact,facfact,pdf1,pdf2")] + "</event>\n"


# Re-indent XML so that elements are on their own line
def reindent_XML(elem):
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = "\n"
        if not elem.tail or not elem.tail.strip():
            elem.tail = "\n"
        for subelem in elem:
            reindent_XML(subelem)
        if not elem.tail or not elem.tail.strip():
            elem.tail = "\n"
    else:
        if not elem.tail or not elem.tail.strip():
            elem.tail = "\n"
