lwtnn Utilities
===============

This is some Atlas-side helpers for the [lwtnn][1] package.

At some point things here might be migrated upstream, but we keep this
package around for development that might not want to wait for
`AtlasExternals` to be updated.


Package Contents
----------------

- `FastGraph`: Faster version of `lwt::LightweightGraph`. Takes
  vectors rather than `std::map<std::string,...>` inputs.

- `InputOrder`: a structure with two fields, `scalar` and `sequence`,
  to specify the input ordering that `FastGraph` should expect for
  scalar and sequence inputs. Each field is of type
  `vector<pair<string,vector<string>>>`. The first string in each
  `pair` names an input node, while the second pair names the
  individual variables in the input arrays.


More details on using FastGraph
-------------------------------

The `FastGraph` class can be initialized as

```C++
std::ifstream nn_file("PATH_TO_JSON_FILE.json");
lwt::GraphConfig config = lwt::parse_json_graph(nn_file);
lwt::atlas::FastGraph graph(config, input_order, default_output);
...
Eigen::VectorXd outputs = graph.compute(input_scalars, input_sequences, output_index);
```

where `input_order` is an `InputOrder` object (see above) which should
specify the order of the inputs that _you_ will feed the network in
`input_scalars` and `input_sequences`. This can be different from the
order which was used in the training.

The `input_scalars` are of type `std::vector<Eigen::VectorXd>`, while
the `input_sequences` are of type `std::vector<Eigen::MatrixXd>`. In
both cases the elements of the `std::vector` correspond to input
nodes, while elements of the `Eigen` object correspond to the
individual input variables within the nodes.

In the sequence case, each row the `Eigen::MatrixXd` corresponds to
one variable, while each column corresponds to one step in the
sequence.


### Output Indices and Defaults

The `default_output` argument is a string specifying the name of the
default output node. It is optional if the network only has only one
output node.

The `output_index` argument is only needed if you want to override the
default. It is specified as an integer to avoid looking up the number
by string at runtime. To cache this index, call

```
size_t cached_index = config.outputs.at("OUTPUT_NODE_NAME").node_index;
```

when you initialize the network. Here `"OUTPUT_NODE_NAME"` should be
replaced with the name you gave the network when training.


### Mismatches Between Variables in Training and Inference

It's up to you to make sure that the `InputOrder` corresponds to
exactly the ordering that you pass to `FastGraph::compute(...)` (the
class has no way to know if you're doing this incorrectly).

But we can support some mismatches between the variables used in
training and those fed into `FastGraph`.

- If, for example you feed the network any inputs which were not used
  in the training, they will be silently ignored.

- It's also fine to use a different ordering in the training and when
  you apply `FastGraph`: the code will rearrange them internally.

- If any variables from the training are _missing_ however, the code
  will crash with an `NNConfigurationException`.

In practice this allows some input evolution without forcing the
inference-side code to accommodate every set of inputs which are used
in production. If, for example, you wish to add a new variable or node
to training "version 2", you can append these to the existing arrays
fed to `FastGraph::compute(...)` without changing the behaviour of
"version 1" (assuming that you keep the correspondence between
`InputOrder` and the input arrays correct). You can also remove
variables from the training with no changes at all to the inference
code.


[1]: https://www.github.com/lwtnn/lwtnn
