

pipeline {
    agent any

    parameters {
        string(name: 'VERSION', defaultValue: '2.3.9', description: 'without v prefix')
    }

    stages {
        stage('build linux-arm64-agent') {
            agent {
                dockerfile {
                    filename 'Dockerfile'
                    label 'linux-arm64-agent'
                }
            }
            steps {
                sh '''
                    echo $VERSION | grep -v v
                    rm *.gz || echo clean
                    ls -ltr
                    sed -i  "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile
                    make clean release
                '''
                stash includes: "*.gz", name: 'linux-arm64-agent'
            }

        }

        stage('build linux-amd64-agent') {
            agent {
                dockerfile {
                    filename 'Dockerfile'
                    label 'linux-amd64-agent'
                }
            }
            steps {
                sh '''
                    rm *.gz || echo clean
                    ls -ltr
                    sed -i  "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile
                    make clean release
                '''
                stash includes: "*.gz", name: 'linux-amd64-agent'
            }

        }
        stage('build linux-amd64-musl-agent') {
            agent {
                dockerfile {
                    filename 'Dockerfile.musl'
                    label 'linux-amd64-agent'
                }
            }
            steps {
                sh '''
                    rm *.gz || echo clean
                    ls -ltr
                    sed -i  "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile
                    make clean release
                '''
                stash includes: "*.gz", name: 'linux-amd64-musl-agent'
            }

        }

        stage('build mac-arm64-agent') {
            agent { label 'mac-arm64-agent' }
            steps {
                sh '''
                    rm *.zip || echo clean
                    ls -ltr
                    sed -i '' "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile
                    export FAT_BINARY=true
                    export JAVA_HOME=/opt/homebrew/opt/openjdk/
                    make clean release
                '''
                stash includes: "*.zip", name: 'mac-arm64-agent'
            }

        }
        stage('commit & publish') {
            agent { label 'linux-amd64-agent' }

            steps {
                sh '''
                    ls -ltr
                    rm *.zip *.gz || echo clean
                    ls -ltr
                '''
                unstash 'linux-arm64-agent'
                unstash 'linux-amd64-agent'
                unstash 'linux-amd64-musl-agent'
                unstash 'mac-arm64-agent'

                sh '''
                   B=$(echo $GIT_BRANCH | sed s/origin.//)
                   git reset --hard HEAD && git checkout $B && git reset --hard $GIT_BRANCH
                   sed -i  "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile
                   git add Makefile
                   git config user.name "Pyroscope Bot"
                   git config user.email "dmitry+bot@pyroscope.io"
                   git commit -m \"version $VERSION\"
                   git tag v$VERSION
                '''

                withCredentials([string(credentialsId: 'github_token', variable: 'GH_TOKEN')]) {
                   sh '''
                       export U=https://pyroscopebot:$GH_TOKEN@github.com/pyroscope-io/async-profiler.git
                       B=$(echo $GIT_BRANCH | sed s/origin.//)
                       git push $U $B
                       git push  --atomic $U "refs/tags/v$VERSION"
                       gh release create "v$VERSION" --title '' --notes ''
                       gh release upload "v$VERSION" *.gz *.zip
                   '''
                }
            }

        }


    }
}

