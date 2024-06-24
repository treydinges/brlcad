#include <cstdio>
#include <fstream>
#include <iostream>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"


//print geometry (possibly)
static std::string PrintMode(int mode) {
  if (mode == TINYGLTF_MODE_POINTS) {
    return "POINTS";
  } else if (mode == TINYGLTF_MODE_LINE) {
    return "LINE";
  } else if (mode == TINYGLTF_MODE_LINE_LOOP) {
    return "LINE_LOOP";
  } else if (mode == TINYGLTF_MODE_TRIANGLES) {
    return "TRIANGLES";
  } else if (mode == TINYGLTF_MODE_TRIANGLE_FAN) {
    return "TRIANGLE_FAN";
  } else if (mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
    return "TRIANGLE_STRIP";
  }
  return "**UNKNOWN**";
}

static std::string PrintComponentType(int ty) {
  if (ty == TINYGLTF_COMPONENT_TYPE_BYTE) {
    return "BYTE";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    return "UNSIGNED_BYTE";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_SHORT) {
    return "SHORT";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
    return "UNSIGNED_SHORT";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_INT) {
    return "INT";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
    return "UNSIGNED_INT";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    return "FLOAT";
  } else if (ty == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
    return "DOUBLE";
  }

  return "**UNKNOWN**";
}

static std::string PrintType(int ty) {
  if (ty == TINYGLTF_TYPE_SCALAR) {
    return "SCALAR";
  } else if (ty == TINYGLTF_TYPE_VECTOR) {
    return "VECTOR";
  } else if (ty == TINYGLTF_TYPE_VEC2) {
    return "VEC2";
  } else if (ty == TINYGLTF_TYPE_VEC3) {
    return "VEC3";
  } else if (ty == TINYGLTF_TYPE_VEC4) {
    return "VEC4";
  } else if (ty == TINYGLTF_TYPE_MATRIX) {
    return "MATRIX";
  } else if (ty == TINYGLTF_TYPE_MAT2) {
    return "MAT2";
  } else if (ty == TINYGLTF_TYPE_MAT3) {
    return "MAT3";
  } else if (ty == TINYGLTF_TYPE_MAT4) {
    return "MAT4";
  }
  return "**UNKNOWN**";
}


//indention
static std::string Indent(const int indent) {
  std::string s;
  for (int i = 0; i < indent; i++) {
    s += "  ";
  }

  return s;
}

static std::string PrintIntArray(const std::vector<int> &arr) {
  if (arr.size() == 0) {
    return "";
  }

  std::stringstream ss;
  ss << "[ ";
  for (size_t i = 0; i < arr.size(); i++) {
    ss << arr[i];
    if (i != arr.size() - 1) {
      ss << ", ";
    }
  }
  ss << " ]";

  return ss.str();
}


static std::string PrintFloatArray(const std::vector<double> &arr) {
  if (arr.size() == 0) {
    return "";
  }

  std::stringstream ss;
  ss << "[ ";
  for (size_t i = 0; i < arr.size(); i++) {
    ss << arr[i];
    if (i != arr.size() - 1) {
      ss << ", ";
    }
  }
  ss << " ]";

  return ss.str();
}

static std::string PrintParameterValue(const tinygltf::Parameter &param) {
  if (!param.number_array.empty()) {
    return PrintFloatArray(param.number_array);
  } else {
    return param.string_value;
  }
}

//prints a value
static std::string PrintValue(const std::string &name,
                              const tinygltf::Value &value, const int indent,
                              const bool tag = true) {
  std::stringstream ss;

  if (value.IsObject()) {
    const tinygltf::Value::Object &o = value.Get<tinygltf::Value::Object>();
    tinygltf::Value::Object::const_iterator it(o.begin());
    tinygltf::Value::Object::const_iterator itEnd(o.end());
    for (; it != itEnd; it++) {
      ss << PrintValue(it->first, it->second, indent + 1) << std::endl;
    }
  } else if (value.IsString()) {
    if (tag) {
      ss << Indent(indent) << name << " : " << value.Get<std::string>();
    } else {
      ss << Indent(indent) << value.Get<std::string>() << " ";
    }
  } else if (value.IsBool()) {
    if (tag) {
      ss << Indent(indent) << name << " : " << value.Get<bool>();
    } else {
      ss << Indent(indent) << value.Get<bool>() << " ";
    }
  } else if (value.IsNumber()) {
    if (tag) {
      ss << Indent(indent) << name << " : " << value.Get<double>();
    } else {
      ss << Indent(indent) << value.Get<double>() << " ";
    }
  } else if (value.IsInt()) {
    if (tag) {
      ss << Indent(indent) << name << " : " << value.Get<int>();
    } else {
      ss << Indent(indent) << value.Get<int>() << " ";
    }
  } else if (value.IsArray()) {
    // TODO(syoyo): Better pretty printing of array item
    ss << Indent(indent) << name << " [ \n";
    for (size_t i = 0; i < value.Size(); i++) {
      ss << PrintValue("", value.Get(int(i)), indent + 1, /* tag */ false);
      if (i != (value.ArrayLen() - 1)) {
        ss << ", \n";
      }
    }
    ss << "\n" << Indent(indent) << "] ";
  }

  // @todo { binary }

  return ss.str();
}

static void DumpStringIntMap(const std::map<std::string, int> &m, int indent) {
  std::map<std::string, int>::const_iterator it(m.begin());
  std::map<std::string, int>::const_iterator itEnd(m.end());
  for (; it != itEnd; it++) {
    std::cout << Indent(indent) << it->first << ": " << it->second << std::endl;
  }
}

static void DumpExtensions(const tinygltf::ExtensionMap &extension,
                           const int indent) {
  // TODO(syoyo): pritty print Value
  for (auto &e : extension) {
    std::cout << Indent(indent) << e.first << std::endl;
    std::cout << PrintValue("extensions", e.second, indent + 1) << std::endl;
  }
}

static void DumpNode(const tinygltf::Node &node, int indent) {
  std::cout << Indent(indent) << "name        : " << node.name << std::endl;
  std::cout << Indent(indent) << "camera      : " << node.camera << std::endl;
  std::cout << Indent(indent) << "mesh        : " << node.mesh << std::endl;
  if (!node.rotation.empty()) {
    std::cout << Indent(indent)
              << "rotation    : " << PrintFloatArray(node.rotation)
              << std::endl;
  }
  if (!node.scale.empty()) {
    std::cout << Indent(indent)
              << "scale       : " << PrintFloatArray(node.scale) << std::endl;
  }
  if (!node.translation.empty()) {
    std::cout << Indent(indent)
              << "translation : " << PrintFloatArray(node.translation)
              << std::endl;
  }

  if (!node.matrix.empty()) {
    std::cout << Indent(indent)
              << "matrix      : " << PrintFloatArray(node.matrix) << std::endl;
  }

  std::cout << Indent(indent)
            << "children    : " << PrintIntArray(node.children) << std::endl;
}

static void DumpPrimitive(const tinygltf::Primitive &primitive, int indent) {
  std::cout << Indent(indent) << "material : " << primitive.material
            << std::endl;
  std::cout << Indent(indent) << "indices : " << primitive.indices << std::endl;
  std::cout << Indent(indent) << "mode     : " << PrintMode(primitive.mode)
            << "(" << primitive.mode << ")" << std::endl;
  std::cout << Indent(indent)
            << "attributes(items=" << primitive.attributes.size() << ")"
            << std::endl;
  DumpStringIntMap(primitive.attributes, indent + 1);

  DumpExtensions(primitive.extensions, indent);
  std::cout << Indent(indent) << "extras :" << std::endl
            << PrintValue("extras", primitive.extras, indent + 1) << std::endl;

  if (!primitive.extensions_json_string.empty()) {
    std::cout << Indent(indent + 1) << "extensions(JSON string) = "
              << primitive.extensions_json_string << "\n";
  }

  if (!primitive.extras_json_string.empty()) {
    std::cout << Indent(indent + 1)
              << "extras(JSON string) = " << primitive.extras_json_string
              << "\n";
  }
}

static void DumpTextureInfo(const tinygltf::TextureInfo &texinfo,
                            const int indent) {
  std::cout << Indent(indent) << "index     : " << texinfo.index << "\n";
  std::cout << Indent(indent) << "texCoord  : TEXCOORD_" << texinfo.texCoord
            << "\n";
  DumpExtensions(texinfo.extensions, indent + 1);
  std::cout << PrintValue("extras", texinfo.extras, indent + 1) << "\n";

  if (!texinfo.extensions_json_string.empty()) {
    std::cout << Indent(indent)
              << "extensions(JSON string) = " << texinfo.extensions_json_string
              << "\n";
  }

  if (!texinfo.extras_json_string.empty()) {
    std::cout << Indent(indent)
              << "extras(JSON string) = " << texinfo.extras_json_string << "\n";
  }
}

static void DumpNormalTextureInfo(const tinygltf::NormalTextureInfo &texinfo,
                                  const int indent) {
  std::cout << Indent(indent) << "index     : " << texinfo.index << "\n";
  std::cout << Indent(indent) << "texCoord  : TEXCOORD_" << texinfo.texCoord
            << "\n";
  std::cout << Indent(indent) << "scale     : " << texinfo.scale << "\n";
  DumpExtensions(texinfo.extensions, indent + 1);
  std::cout << PrintValue("extras", texinfo.extras, indent + 1) << "\n";
}

static void DumpOcclusionTextureInfo(
    const tinygltf::OcclusionTextureInfo &texinfo, const int indent) {
  std::cout << Indent(indent) << "index     : " << texinfo.index << "\n";
  std::cout << Indent(indent) << "texCoord  : TEXCOORD_" << texinfo.texCoord
            << "\n";
  std::cout << Indent(indent) << "strength  : " << texinfo.strength << "\n";
  DumpExtensions(texinfo.extensions, indent + 1);
  std::cout << PrintValue("extras", texinfo.extras, indent + 1) << "\n";
}

static void DumpPbrMetallicRoughness(const tinygltf::PbrMetallicRoughness &pbr,
                                     const int indent) {
  std::cout << Indent(indent)
            << "baseColorFactor   : " << PrintFloatArray(pbr.baseColorFactor)
            << "\n";
  std::cout << Indent(indent) << "baseColorTexture  :\n";
  DumpTextureInfo(pbr.baseColorTexture, indent + 1);

  std::cout << Indent(indent) << "metallicFactor    : " << pbr.metallicFactor
            << "\n";
  std::cout << Indent(indent) << "roughnessFactor   : " << pbr.roughnessFactor
            << "\n";

  std::cout << Indent(indent) << "metallicRoughnessTexture  :\n";
  DumpTextureInfo(pbr.metallicRoughnessTexture, indent + 1);
  DumpExtensions(pbr.extensions, indent + 1);
  std::cout << PrintValue("extras", pbr.extras, indent + 1) << "\n";
}
//
// This is where the main function starts
//
//
//
//
//rename dump and all we want is the mesh and material
static void Dump(const tinygltf::Model &model) 
{
    std::cout << "=== Dump glTF ===" << std::endl;
    std::cout << "asset.copyright          : " << model.asset.copyright
                << std::endl;
    std::cout << "asset.generator          : " << model.asset.generator
                << std::endl;
    std::cout << "asset.version            : " << model.asset.version
                << std::endl;
    std::cout << "asset.minVersion         : " << model.asset.minVersion
                << std::endl;
    std::cout << std::endl;

    std::cout << "=== Dump scene ===" << std::endl;
    std::cout << "defaultScene: " << model.defaultScene << std::endl;

    {
        std::cout << "scenes(items=" << model.scenes.size() << ")" << std::endl;
        for (size_t i = 0; i < model.scenes.size(); i++) {
        std::cout << Indent(1) << "scene[" << i
                    << "] name  : " << model.scenes[i].name << std::endl;
        DumpExtensions(model.scenes[i].extensions, 1);
        }
    }

     { //meshes
        std::cout << "meshes(item=" << model.meshes.size() << ")" << std::endl;
        for (size_t i = 0; i < model.meshes.size(); i++) {
        std::cout << Indent(1) << "name     : " << model.meshes[i].name
                    << std::endl;
        std::cout << Indent(1)
                    << "primitives(items=" << model.meshes[i].primitives.size()
                    << "): " << std::endl;

        for (size_t k = 0; k < model.meshes[i].primitives.size(); k++) {
            DumpPrimitive(model.meshes[i].primitives[k], 2);
        }
      }
    }

    {
    for (size_t i = 0; i < model.accessors.size(); i++) {
      const tinygltf::Accessor &accessor = model.accessors[i];
      std::cout << Indent(1) << "name         : " << accessor.name << std::endl;
      std::cout << Indent(2) << "bufferView   : " << accessor.bufferView
                << std::endl;
      std::cout << Indent(2) << "byteOffset   : " << accessor.byteOffset
                << std::endl;
      std::cout << Indent(2) << "componentType: "
                << PrintComponentType(accessor.componentType) << "("
                << accessor.componentType << ")" << std::endl;
      std::cout << Indent(2) << "count        : " << accessor.count
                << std::endl;
      std::cout << Indent(2) << "type         : " << PrintType(accessor.type)
                << std::endl;
      if (!accessor.minValues.empty()) {
        std::cout << Indent(2) << "min          : [";
        for (size_t k = 0; k < accessor.minValues.size(); k++) {
          std::cout << accessor.minValues[k]
                    << ((k != accessor.minValues.size() - 1) ? ", " : "");
        }
        std::cout << "]" << std::endl;
      }
      if (!accessor.maxValues.empty()) {
        std::cout << Indent(2) << "max          : [";
        for (size_t k = 0; k < accessor.maxValues.size(); k++) {
          std::cout << accessor.maxValues[k]
                    << ((k != accessor.maxValues.size() - 1) ? ", " : "");
        }
        std::cout << "]" << std::endl;
      }

      if (accessor.sparse.isSparse) {
        std::cout << Indent(2) << "sparse:" << std::endl;
        std::cout << Indent(3) << "count  : " << accessor.sparse.count
                  << std::endl;
        std::cout << Indent(3) << "indices: " << std::endl;
        std::cout << Indent(4)
                  << "bufferView   : " << accessor.sparse.indices.bufferView
                  << std::endl;
        std::cout << Indent(4)
                  << "byteOffset   : " << accessor.sparse.indices.byteOffset
                  << std::endl;
        std::cout << Indent(4) << "componentType: "
                  << PrintComponentType(accessor.sparse.indices.componentType)
                  << "(" << accessor.sparse.indices.componentType << ")"
                  << std::endl;
        std::cout << Indent(3) << "values : " << std::endl;
        std::cout << Indent(4)
                  << "bufferView   : " << accessor.sparse.values.bufferView
                  << std::endl;
        std::cout << Indent(4)
                  << "byteOffset   : " << accessor.sparse.values.byteOffset
                  << std::endl;
      }
    }
  }


//nodes
 {
    std::cout << "nodes(items=" << model.nodes.size() << ")" << std::endl;
    for (size_t i = 0; i < model.nodes.size(); i++) {
      const tinygltf::Node &node = model.nodes[i];
      std::cout << Indent(1) << "name         : " << node.name << std::endl;

      DumpNode(node, 2);
    }
  }

{
    std::cout << "materials(items=" << model.materials.size() << ")"
              << std::endl;
    for (size_t i = 0; i < model.materials.size(); i++) {
      const tinygltf::Material &material = model.materials[i];
      std::cout << Indent(1) << "name                 : " << material.name
                << std::endl;

      std::cout << Indent(1) << "alphaMode            : " << material.alphaMode
                << std::endl;
      std::cout << Indent(1)
                << "alphaCutoff          : " << material.alphaCutoff
                << std::endl;
      std::cout << Indent(1) << "doubleSided          : "
                << (material.doubleSided ? "true" : "false") << std::endl;
      std::cout << Indent(1) << "emissiveFactor       : "
                << PrintFloatArray(material.emissiveFactor) << std::endl;

      std::cout << Indent(1) << "pbrMetallicRoughness :\n";
      DumpPbrMetallicRoughness(material.pbrMetallicRoughness, 2);

      std::cout << Indent(1) << "normalTexture        :\n";
      DumpNormalTextureInfo(material.normalTexture, 2);

      std::cout << Indent(1) << "occlusionTexture     :\n";
      DumpOcclusionTextureInfo(material.occlusionTexture, 2);

      std::cout << Indent(1) << "emissiveTexture      :\n";
      DumpTextureInfo(material.emissiveTexture, 2);

      std::cout << Indent(1) << "----  legacy material parameter  ----\n";
      std::cout << Indent(1) << "values(items=" << material.values.size() << ")"
                << std::endl;
      tinygltf::ParameterMap::const_iterator p(material.values.begin());
      tinygltf::ParameterMap::const_iterator pEnd(material.values.end());
      for (; p != pEnd; p++) {
        std::cout << Indent(2) << p->first << ": "
                  << PrintParameterValue(p->second) << std::endl;
      }
      std::cout << Indent(1) << "-------------------------------------\n";

      DumpExtensions(material.extensions, 1);
      std::cout << PrintValue("extras", material.extras, 2) << std::endl;

      if (!material.extensions_json_string.empty()) {
        std::cout << Indent(2) << "extensions(JSON string) = "
                  << material.extensions_json_string << "\n";
      }

      if (!material.extras_json_string.empty()) {
        std::cout << Indent(2)
                  << "extras(JSON string) = " << material.extras_json_string
                  << "\n";
      }
    }
  }


// toplevel extensions
  {
    std::cout << "extensions(items=" << model.extensions.size() << ")"
              << std::endl;
    DumpExtensions(model.extensions, 1);
  }

}




static std::string GetFilePathExtension(const std::string &FileName) 
{
  if (FileName.find_last_of(".") != std::string::npos)
  {
    return FileName.substr(FileName.find_last_of(".") + 1);
  }
  return "";
}


int main(int argc, char **argv)
{
    bool store_original_json_for_extras_and_extensions = false;
    
    if (argc < 2) 
    {
        printf("Needs input.gltf\n");
        exit(1);
    }
    
    if (argc > 2) {
        store_original_json_for_extras_and_extensions = true;
    }
    

    tinygltf::Model model;
    std::string err;
    std::string warn;
    std::string input_filename(argv[1]);
    std::string ext = GetFilePathExtension(input_filename);

    bool ret = false;

    // getting undefined reference here for the class
    tinygltf::TinyGLTF gltf_ctx;



    
    gltf_ctx.SetStoreOriginalJSONForExtrasAndExtensions(
        store_original_json_for_extras_and_extensions);

    


    if (ext.compare("glb") == 0) {
    std::cout << "Reading binary glTF" << std::endl;
    // assume binary glTF.
    ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn,
                                      input_filename.c_str());
  } else {
    std::cout << "Reading ASCII glTF" << std::endl;
    // assume ascii glTF.
    ret =
        gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, input_filename.c_str());
  }


    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
        return -1;
    }

    printf("check\n");

    //test dump
    Dump(model);

    return 0;


}

