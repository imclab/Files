#ifndef PQRS_XML_COMPILER_HPP
#define PQRS_XML_COMPILER_HPP

#include <string>
#include <stdexcept>
#include <vector>
#include <tr1/memory>
#include <tr1/unordered_map>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include "pqrs/string.hpp"

namespace pqrs {
  class xml_compiler {
  public:
    xml_compiler(const std::string& system_xml_directory, const std::string& private_xml_directory);

    void reload(void);

    const std::string& get_error_message(void) const;
    int get_error_count(void) const;

    boost::optional<uint32_t> find_symbol_map(const std::string& name) const;

    static void normalize_identifier(std::string& identifier);

    // ============================================================
    class xml_compiler_runtime_error : public std::runtime_error {
    public:
      xml_compiler_runtime_error(const std::string& what) : std::runtime_error(what) {}
    };
    class xml_compiler_logic_error : public std::logic_error {
    public:
      xml_compiler_logic_error(const std::string& what) : std::logic_error(what) {}
    };

    class xml_file_path {
    public:
      class base_directory {
      public:
        enum type {
          system_xml,
          private_xml,
        };
      };

      xml_file_path(base_directory::type base_directory_type, const std::string& relative_path) :
        base_directory_type_(base_directory_type),
        relative_path_(relative_path)
      {}

      base_directory::type get_base_directory(void) const { return base_directory_type_; }
      const std::string& get_relative_path(void) const { return relative_path_; }

    private:
      base_directory::type base_directory_type_;
      const std::string relative_path_;
    };
    typedef std::tr1::shared_ptr<xml_file_path> xml_file_path_ptr;

    class symbol_map {
    public:
      symbol_map(void);
      void clear(void);

      uint32_t get(const std::string& name) const;
      uint32_t get(const std::string& type, const std::string& name) const;

      boost::optional<uint32_t> get_optional(const std::string& name) const;
      boost::optional<uint32_t> get_optional(const std::string& type, const std::string& name) const;

      bool exists(const std::string& name) const;
      bool exists(const std::string& type, const std::string& name) const;

      // Call add("KeyCode", "RETURN", 36) to register "KeyCode::RETURN = 36".
      void add(const std::string& type, const std::string& name, uint32_t value);
      void add(const std::string& type, const std::string& name);

    private:
      std::tr1::unordered_map<std::string, uint32_t> symbol_map_;
    };

    class appdef {
    public:
      const std::string& get_name(void) const { return name_; }
      void set_name(const std::string& v) { name_ = v; }
      void add_rule_equal(const std::string& v);
      void add_rule_prefix(const std::string& v);
      bool is_rules_matched(const std::string& identifier);

    private:
      std::string name_;
      std::vector<std::string> rules_equal_;
      std::vector<std::string> rules_prefix_;
    };

    class remapclasses_initialize_vector {
    public:
      remapclasses_initialize_vector(void);
      void clear(void);
      const std::vector<uint32_t>& get(void) const;
      void add(const std::vector<uint32_t>& v, uint32_t configindex);
      void freeze(void);

    private:
      enum {
        INDEX_OF_FORMAT_VERSION = 0,
        INDEX_OF_COUNT = 1,
      };

      std::vector<uint32_t> data_;
      std::tr1::unordered_map<uint32_t, bool> is_configindex_added_;
      uint32_t max_configindex_;
      bool freezed_;
    };

    class filter_vector {
    public:
      filter_vector(void);
      filter_vector(const symbol_map& symbol_map, const boost::property_tree::ptree& pt);
      std::vector<uint32_t>& get(void);
      const std::vector<uint32_t>& get(void) const;
      bool empty(void) const;

    private:
      void add(const symbol_map& symbol_map, uint32_t filter_type, const std::string& type, const std::string& string);

      std::vector<uint32_t> data_;
    };

  private:
    typedef std::tr1::shared_ptr<boost::property_tree::ptree> ptree_ptr;
    void read_xmls_(std::vector<ptree_ptr>& pt_ptrs, const std::vector<xml_file_path_ptr>& xml_file_path_ptrs);

    void set_error_message_(const std::string& message);

    void reload_replacementdef_(void);
    void traverse_replacementdef_(const boost::property_tree::ptree& pt);

    void reload_symbol_map_(void);
    void traverse_symbol_map_(const boost::property_tree::ptree& pt);

    void reload_appdef_(void);
    void traverse_appdef_(const boost::property_tree::ptree& pt);

    void reload_devicedef_(void);
    void traverse_devicedef_(const boost::property_tree::ptree& pt);

    void reload_autogen_(void);
    void add_configindex_and_keycode_to_symbol_map_(const boost::property_tree::ptree& pt, bool handle_notsave);
    void traverse_identifier_(const boost::property_tree::ptree& pt);
    void traverse_autogen_(const boost::property_tree::ptree& pt,
                           const std::string& identifier,
                           const filter_vector& filter_vector,
                           std::vector<uint32_t>& initialize_vector);
    void handle_autogen(const std::string& autogen,
                        const filter_vector& filter_vector,
                        std::vector<uint32_t>& initialize_vector);
    void add_to_initialize_vector(const std::string& params,
                                  uint32_t type,
                                  const filter_vector& filter_vector,
                                  std::vector<uint32_t>& initialize_vector);

    const std::string system_xml_directory_;
    const std::string private_xml_directory_;

    std::string error_message_;
    int error_count_;

    symbol_map symbol_map_;
    pqrs::string::replacement replacement_;
    std::tr1::unordered_map<uint32_t, std::string> confignamemap_;
    remapclasses_initialize_vector remapclasses_initialize_vector_;
    uint32_t simultaneous_keycode_index_;

    std::vector<std::tr1::shared_ptr<appdef> > app_;
  };
}

#endif