{
    "targets": [{
        "target_name": "event-source-base-test",
        "sources": [ 
            'addon.cpp',
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        "cflags": ["-Wall", "-std=c++17"],
        'cflags_cc': ["-Wall", "-std=c++17"],
        'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        'link_settings': {
            "libraries": [ 
            ]
        }          
    }]
}

