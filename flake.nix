{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};

        in
        {
          devShells.default = pkgs.stdenv.mkDerivation {
            name = "yukikaze";
            src = self;
            buildInputs = with pkgs; [
              libgcc
              protobuf
              grpc
              openssl
              gnumake
              cmake
              docker
            ];
          };
        }
      );
}