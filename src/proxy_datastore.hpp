/*
 * Copyright (C) 2020 CESNET, https://photonics.cesnet.cz/
 *
 * Written by Václav Kubernát <kubernat@cesnet.cz>
 *
*/

#pragma once
#include "datastore_access.hpp"
#include "yang_access.hpp"

/*! \class ProxyDatastore
 *     \brief DatastoreAccess wrapper that handles RPC input
 */
class ProxyDatastore {
public:
    /**
     * The createTemporaryDatastore function should create a temporary datastore that's going to be used for RPC input.
     * This temporary datastore and the main datastore are supposed share the same schemas.
     * */
    ProxyDatastore(const std::shared_ptr<DatastoreAccess>& datastore, std::function<std::shared_ptr<DatastoreAccess>(const std::shared_ptr<DatastoreAccess>&)> createTemporaryDatastore);
    [[nodiscard]] DatastoreAccess::Tree getItems(const std::string& path) const;
    void setLeaf(const std::string& path, leaf_data_ value);
    void createItem(const std::string& path);
    void deleteItem(const std::string& path);
    void moveItem(const std::string& source, std::variant<yang::move::Absolute, yang::move::Relative> move);
    void commitChanges();
    void discardChanges();
    void copyConfig(const Datastore source, const Datastore destination);
    [[nodiscard]] std::string dump(const DataFormat format) const;

    void initiateRpc(const std::string& rpcPath);
    void initiateAction(const std::string& actionPath);
    [[nodiscard]] DatastoreAccess::Tree execute();
    void cancel();

    [[nodiscard]] std::shared_ptr<Schema> schema() const;
private:
    /** @brief Picks a datastore based on the requested path.
     *
     * If the path starts with a currently processed RPC, m_inputDatastore is picked.
     * Otherwise m_datastore is picked.
     */
    [[nodiscard]] std::shared_ptr<DatastoreAccess> pickDatastore(const std::string& path) const;

    std::shared_ptr<DatastoreAccess> m_datastore;
    std::function<std::shared_ptr<DatastoreAccess>(const std::shared_ptr<DatastoreAccess>&)> m_createTemporaryDatastore;
    std::shared_ptr<DatastoreAccess> m_inputDatastore;

    struct RpcInput {
        std::string m_path;
    };

    struct ActionInput {
        std::string m_path;
    };
    // This variant is needed, so that I know whether to call executeRpc or executeAction
    // TODO: get rid of this variant with sysrepo2 because the method for RPC/action is the same there
    std::variant<ActionInput, RpcInput> m_inputPath;
};
