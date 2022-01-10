# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""! Utility functions for manipulating LHE files.

@author James Robinson  <james.robinson@cern.ch>
"""
import re
import glob
import mmap
from AthenaCommon import Logging
from xml.etree import ElementTree

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


def merge(input_file_pattern, output_file):
    """! Merge many input LHE files into a single output file."""
    # Create input file list from pattern, ensuring that output_file is not in the list
    input_file_list = sorted([x for x in glob.glob(input_file_pattern) if not x == output_file])
    if len(input_file_list) < 1:
        raise IOError("No input LHE files provided. Aborting merge!")
    logger.info("Preparing to create {} from {} input files".format(output_file, len(input_file_list)))

    # Open output file and write events
    nEvents = 0
    with open(output_file, "ab") as f_output:
        logger.info("... reading metadata from {}".format(input_file_list[0]))
        # Start with the first file and extract opening/closing string
        s_preamble = preamble(input_file_list[0])
        s_postamble = postamble(input_file_list[0])
        f_output.write(s_preamble)

        # Now write events from all files to output
        # Use sanitised list in case output_file matches the pattern
        for nEvents, event in enumerate(event_iterator(input_file_list)):
            f_output.write(event)

        # Finally add the footer
        f_output.write(s_postamble)

    # Write the event count to the logger
    logger.info("Wrote {} events to {}".format(nEvents + 1, output_file))


def event_iterator(input_files, verbose=True):
    """! Python generator to iterate through events from input LHE files."""
    # If a glob-able pattern is provided, expand this into a list
    if not isinstance(input_files, list):
        input_files = sorted(glob.glob(input_files))

    # Iterate over input files
    for file_name in input_files:
        if verbose:
            logger.info("... reading events from {}".format(file_name))
        in_event, event_lines = False, ""
        # Group all lines inside an XML event element
        with open(file_name, "r") as f_input:
            for line in f_input:
                # Both <event ...> and <event> are permitted
                if "<event" in line:
                    in_event = True
                    line = line[line.index("<event"):]  # catch cases like "</init><event>"
                if in_event:
                    event_lines += line
                if "</event>" in line:
                    yield event_lines
                    in_event, event_lines = False, ""


def event_weight_iterator(input_files):
    """! Python generator to iterate through event weights from input LHE files."""
    for event in event_iterator(input_files):
        yield float([w for w in event.splitlines()[1].split(" ") if w][2])


def event_counter(input_file_pattern):
    """! Count total number of events in input files."""
    for nEvents, _ in enumerate(event_iterator(input_file_pattern, verbose=False)):
        pass
    return nEvents + 1


def add_weight_to_header(header, weightgroup_name, weight_name, weight_id):
    """! Add a weight to a header passed as input (can be a string or an ElementTree)."""
    # Convert string to ElementTree
    if not isinstance(header, ElementTree.Element):
        header_elem = ElementTree.fromstring(header)
    else:
        header_elem = header

    # Add initrwgt element if it doesn't exist
    if header_elem.find("initrwgt") is None:
        header_elem.append(ElementTree.fromstring("<initrwgt></initrwgt>"))

    # Add weightgroup element if it doesn't exist
    if weightgroup_name not in [x.attrib["name"] for x in header_elem.find("initrwgt").findall("weightgroup")]:
        header_elem.find("initrwgt").append(ElementTree.fromstring("<weightgroup name='{}' combine='None' ></weightgroup>".format(weightgroup_name)))

    # Add weight to appropriate weightgroup
    weightgroup = [x for x in header_elem.find("initrwgt").findall("weightgroup") if x.attrib["name"] == weightgroup_name][0]
    if not any([weight.attrib["id"] == weight_id for weight in weightgroup.findall("weight")]):
        weightgroup.append(ElementTree.fromstring("<weight id='{}'>{}</weight>".format(weight_id, weight_name)))

    reindent_XML(header_elem)
    return header_elem


def preamble(input_LHE_file):
    """! Get opening lines from file as a string."""
    with open(input_LHE_file, "r") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        s_output = s_input[: s_input.find(b"<event>")]
    return "".join( chr(x) for x in s_output)


def postamble(input_LHE_file):
    """! Get closing lines from file as a string."""
    with open(input_LHE_file, "r") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        s_output = s_input[s_input.rfind(b"</event>") + 9:]
    return "".join( chr(x) for x in s_output)


def opening_tag(input_LHE_file):
    """! Get <LesHouchesEvents> opening tag from file as a string."""
    s_opening = preamble(input_LHE_file)
    if s_opening.find("<LesHouchesEvents") != -1:
        return s_opening[s_opening.find("<LesHouchesEvents"): s_opening.find(">") + 1].strip("\n")
    return ""


def comment_block(input_LHE_file):
    """! Get comment block from file as a string."""
    s_opening = preamble(input_LHE_file)
    if s_opening.find("<!--") != -1:
        return s_opening[s_opening.find("<!--"): s_opening.find("-->") + 3].strip("\n")
    return ""


def header_block(input_LHE_file):
    """! Get <header> block from file as a string."""
    s_opening = preamble(input_LHE_file)
    if s_opening.find("<header>") != -1:
        return s_opening[s_opening.find("<header>"): s_opening.find("</header>") + 9].strip("\n")
    return "<header>\n</header>"


def init_block(input_LHE_file):
    """! Get <init> block from file as a string."""
    s_opening = preamble(input_LHE_file)
    # Both <init ...> and <init> are permitted
    if s_opening.find("<init>") != -1:
        return s_opening[s_opening.find("<init>"): s_opening.find("</init>") + 7].strip("\n")
    if s_opening.find("<init ") != -1:
        return s_opening[s_opening.find("<init "): s_opening.find("</init>") + 7].strip("\n")
    return "<init>\n</init>"


def get_first_event(input_LHE_file):
    """! Get first event from file as a string."""
    with open(input_LHE_file, "r") as f_input:
        s_input = mmap.mmap(f_input.fileno(), 0, access=mmap.ACCESS_READ)
        # Both <event ...> and <event> are permitted
        s_output = s_input[s_input.find("<event"): s_input.find("</event>") + 8]
    return "".join( chr(x) for x in s_output)


def string_to_weight(input_event):
    """! Get weight name/value pairs from an input string."""
    comment_lines = input_event[input_event.find("#"):].replace("\n", " ").replace("</event>", "")
    weight_lines = [" ".join(line.split()) for line in comment_lines.split("#") if "new weight,renfact,facfact,pdf1,pdf2" in line]
    return [(line.split(" ")[-1], line.split(" ")[2]) for line in weight_lines]


def ensure_coloured_quarks(input_event):
    """! Ensure that all final-state quarks in the event are coloured."""
    initial_colour_flow, is_event_changed = -1, False
    event_lines = ""
    for input_line in input_event.splitlines(True):
        output_line = None
        try:  # interpret line as a particle
            IDUP, ISTUP, MOTHUP0, MOTHUP1, ICOLUP0, ICOLUP1, PUP0, PUP1, PUP2, PUPU3, PUP4, VTIMUP, SPINUP = input_line.split()
            if int(IDUP) == 21 and int(ISTUP) == -1:  # this is an initial state gluon
                initial_colour_flow = max(initial_colour_flow, int(ICOLUP0), int(ICOLUP1))
            if int(ICOLUP0) == 0 and int(ICOLUP0) == 0:  # this is a colourless particle
                tokens = re.split(r"(\s+)", input_line)
                if int(IDUP) in range(1, 7):  # this is a quark
                    output_line = "".join(tokens[:9]) + " {0:>5d} {1:>5d}".format(initial_colour_flow + 1, 0) + "".join(tokens[13:])
                    is_event_changed = True
                if int(IDUP) in range(-6, 0):  # this is an anti-quark
                    output_line = "".join(tokens[:9]) + " {0:>5d} {1:>5d}".format(0, initial_colour_flow + 1) + "".join(tokens[13:])
                    is_event_changed = True
        except ValueError:  # this is not a particle line
            pass
        event_lines += output_line if output_line is not None else input_line
    return (is_event_changed, event_lines)

def mu2tau(input_event):
    """! 
    Swap out muons for taus, and muon neutrinos for tau neutrinos.
    Note no momentum reshuffling is done, but Pythia appears to restore the correct tau mass.
    """
    is_event_changed = False
    event_lines = ""
    for input_line in input_event.splitlines(True):
        output_line = None
        try:  # interpret line as a particle
            tokens = re.split(r"(\s+)", input_line)
            if len(tokens) < 25: raise ValueError
            IDUP = int(tokens[2])
            if abs(IDUP) == 13 or abs(IDUP) == 14:  # this is a muon or muon neutrino
                if IDUP > 0:
                    IDUP += 2
                else:
                    IDUP -= 2
                is_event_changed = True
                output_line = "".join("".join(tokens[:2])+str(IDUP)+"".join(tokens[3:]))
                # print "LHE mu2tau swap \n\'"+input_line+"\', for \n\'"+output_line+"\'"
        except ValueError:  # this is not a particle line
            pass
        event_lines += output_line if output_line is not None else input_line
    return (is_event_changed, event_lines)


def update_XWGTUP_with_reweighted_nominal(input_event, wgtid_for_old_XWGTUP_value = None):
    """! Ensure that XWGTUP is equal to the reweighted nominal."""
    event_lines = ""
    rwgt_nominal = None
    XWGTUP = None
    for input_line in input_event.splitlines(True): # loop first to fine reweighted nominal
        if input_line.find("<wgt id='0'>") < 0:
            continue
        else:
            try:
                rwgt_nominal = ElementTree.fromstring(input_line.strip())
                break # no need to continue the loop
            except Exception:
                raise IOError("Impossible to understand line with nominal weight from reweighting")
    for input_line in input_event.splitlines(True):
        output_line = None
        if XWGTUP is None: # XWGTUP not yet found
            try:  # interpret line as a general event info line
                NUP, IDPRUP, XWGTUP, SCALUP, AQEDUP, AQCDUP = input_line.split()
                output_line = "      %s  %s  %s  %s %s  %s\n"%(NUP, IDPRUP, rwgt_nominal.text, SCALUP, AQEDUP, AQCDUP)
            except ValueError:  # this is not a general event info line
                pass
        if input_line.find("</rwgt>") >= 0:
            if (wgtid_for_old_XWGTUP_value is not None):# in this case, add the original value of XWGTUP as last weight
                output_line = "<wgt id='%i'>%s</wgt>\n"%(wgtid_for_old_XWGTUP_value, XWGTUP)
                output_line += input_line
        event_lines += output_line if output_line is not None else input_line
    return (event_lines)


def Powheg2LHEv3(input_event, name_to_ID):
    """! Get new-style event weights from an input event string."""
    # Return event as-is if there are no Powheg-style weights
    if "#new weight,renfact,facfact,pdf1,pdf2" not in input_event:
        return input_event
    # Otherwise convert these to LHEv3 weights
    if "<rwgt>" not in input_event:
        nominal_weight = [x for x in input_event.split("\n")[1].split(" ") if len(x) > 0][2]
        input_event = input_event[:input_event.find("#")] + "<rwgt>\n<wgt id='0'> {0} </wgt>\n</rwgt>\n".format(nominal_weight) + input_event[input_event.find("#"):]
    weight_lines = "".join(["<wgt id='{}'> {} </wgt>\n".format(name_to_ID[weight[0]], weight[1]) for weight in string_to_weight(input_event)])
    return input_event[:input_event.find("</rwgt>")] + weight_lines + input_event[input_event.find("</rwgt>"):input_event.find("#new weight,renfact,facfact,pdf1,pdf2")] + "</event>\n"


def reindent_XML(elem):
    """! Re-indent XML so that elements are on their own line."""
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
