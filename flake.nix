{
  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixos-unstable-small/nixexprs.tar.xz";
    flake-parts.url = "github:hercules-ci/flake-parts";
    devenv.url = "github:cachix/devenv";
  };

  outputs = inputs: inputs.flake-parts.lib.mkFlake { inherit inputs; } {
    imports = [
      inputs.devenv.flakeModule
    ];
    systems = inputs.nixpkgs.lib.systems.flakeExposed;
    perSystem = { config, self', inputs', pkgs, system, ... }: {
      devenv.shells.default = {
        packages = with pkgs; [
          gtest
          gtest.dev
          uv
          (python3.withPackages (ps: with ps; [
            fastapi
            uvicorn
            pybind11
            requests
            pyqt6
            matplotlib
            websockets
          ]))
        ];
        languages.python = {
          enable = true;
        };
      };
    };
  };
}
