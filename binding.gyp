{
  'variables': {
    'openssl_fips' : '0'
  },
  'targets': [
    {
      'target_name': 'EventLog',
      'product_extension': 'node',
      'type': 'loadable_module',
      'include_dirs': ['<!(node -p "require(\'node-addon-api\').include_dir")'],
      'defines': [
        '_UNICODE',
        'UNICODE',
        'NODE_ADDON_API_ENABLE_MAYBE',
        'NAPI_DISABLE_CPP_EXCEPTIONS'
      ],
      'libraries': ['advapi32.lib'],
      'configurations': {
        'Release': {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1,
            }
          }
        }
      },
      'conditions': [
        ['OS=="win"', {'sources': ['src/CredentialManager.cpp', 'src/CredentialManagerImpl.cpp', 'src/Conversions.cpp']}]
      ]
    }
  ]
}
