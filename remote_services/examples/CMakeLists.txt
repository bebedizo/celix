# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

celix_subproject(RSA_EXAMPLES "Option to enable building the RSA examples" OFF DEPS LAUNCHER shell_tui log_writer RSA_TOPOLOGY_MANAGER)
if (RSA_EXAMPLES)
    add_subdirectory(calculator_service)

    add_subdirectory(calculator_endpoint)
    add_subdirectory(calculator_endpoint2)

    add_subdirectory(calculator_proxy)
    add_subdirectory(calculator_proxy2)

    add_subdirectory(calculator_shell)
endif (RSA_EXAMPLES)

