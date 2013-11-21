#!/usr/bin/env python

from os import makedirs
from os.path import isdir
from argparse import ArgumentParser

from DQDefects import DefectsDB

DEPENDSON_DIR = "dependson"

class Node(object):
    def __init__(self, name):
        self.name = name
        self.children = set()
        self.parents = set()

    def get_name(self):
        return self.name

    @property
    def virtual(self):
        return bool(self.children)

    @property
    def primary(self):
        return not self.virtual

    @property
    def has_primary_children(self):
        return any(c.primary for c in self.children)

    @property
    def only_primary_children(self):
        return all(c.primary for c in self.children)

    def __repr__(self):
        args = self.name, [n.name for n in self.parents], [n.name for n in self.children]
        return '<Node %s parents="%s" children="%s">' % args

    def dot(self, current_node=False, tooltip="", viewing_dependents=False):
        color = "grey" if self.primary else "white"
        if current_node:
            color = "darkslategray2" if viewing_dependents else "gold"

        tooltip = ("[Virtual]" if self.virtual else "[Primary]") + " " + tooltip
        label = self.name


        if current_node:
            # Clicking on the current node toggles between views
            if viewing_dependents:
                url = "../%s.svg" % self.name
                label += r"\n[Deep dependants]"
            else:
                url = "%s/%s.svg" % (DEPENDSON_DIR, self.name)
                label += r"\n[Dependencies]"
        else:
            url = "%s.svg" % self.name

        args = self.name, color, url, tooltip, label
        return '%s [fillcolor=%s, style=filled, URL="%s", tooltip="%s", label="%s"];' % args

def build_tree(all_logic):

    all_nodes = {}

    def make_primary(current_logic):
        if current_logic in all_nodes:
            return all_nodes[current_logic]
        all_nodes[current_logic] = node = Node(current_logic)
        return node

    def explore_virtual(current_logic):
        if current_logic in all_nodes:
            return all_nodes[current_logic]

        this_logic = all_logic[current_logic]
        all_nodes[current_logic] = node = Node(current_logic)

        for name in this_logic.clauses:
            if name in all_logic:
                child = explore_virtual(name)
            else:
                child = make_primary(name)

            child.parents.add(node)
            node.children.add(child)

        return node

    for name in all_logic:
        explore_virtual(name)

    return all_nodes

def walk(node, visited, visit_primary, fd):
    visited.add(node)

    for subnode in sorted(node.children, key=Node.get_name):
        if subnode.virtual or (subnode.primary and visit_primary):
            print >>fd, "%s -> %s" % (node.name, subnode.name)
            walk(subnode, visited, visit_primary, fd)

def dump_visited(fd, current_node, nodes, descs, parents):
    for node in sorted(nodes, key=Node.get_name):
        at_current_node = node.name == current_node.name
        description = descs.get(node.name, node.name)
        # Line breaks in defect descriptions mess up the svg generation
        description = description.replace('\n', ' ')
        print >>fd, node.dot(at_current_node, description, parents)

def build_dependency_graph(output_dir, node, descs):

    with open("%s/%s.dot" % (output_dir, node.name), "w") as fd:
        print >>fd, "strict digraph %s {" % (node.name)
        print >>fd, "rankdir=LR;\n"

        visited = set()

        for parent in sorted(node.parents, key=Node.get_name):
            print >>fd, "%s -> %s" % (parent.name, node.name)
            visited.add(parent)

        walk(node, visited, node.has_primary_children, fd)

        dump_visited(fd, node, visited, descs, parents=False)

        print >>fd, "}"

def build_parent_tree(output_dir, node, descs):

    visited = set([node])

    with open("%s/%s/%s.dot" % (output_dir, DEPENDSON_DIR, node.name), "w") as fd:
        def walk_parents(node):
            visited.add(node)
            for parent in node.parents:
                print >>fd, "%s -> %s" % (parent.name, node.name)
                walk_parents(parent)

        print >>fd, "strict digraph %s {" % (node.name)
        print >>fd, "rankdir=LR;\n"

        walk_parents(node)

        dump_visited(fd, node, visited, descs, parents=True)

        print >>fd, "}"

def render_all_flags(output_dir, all_nodes, descs):

    path = "%s/%s" % (output_dir, DEPENDSON_DIR)
    if not isdir(path):
        makedirs(path)

    for name, node in sorted(all_nodes.iteritems()):
        build_dependency_graph(output_dir, node, descs)
        build_parent_tree(output_dir, node, descs)

def main():

    parser = ArgumentParser(description="Dump defect viewer information")

    parser.add_argument("-t", "--tag", default="HEAD", help="Defect tag")
    parser.add_argument("-o", "--output", default=".", help="Directory to dump files")

    args = parser.parse_args()

    # Instantiate the Defect DB
    db = DefectsDB(tag=args.tag)

    descs = db.all_defect_descriptions

    # Build node objects
    all_nodes = build_tree(db.virtual_defect_logics)

    # Write outputs
    output_dir = "%s/%s" % (args.output, args.tag)
    render_all_flags(output_dir, all_nodes, descs)

if __name__ == "__main__":
    main()
